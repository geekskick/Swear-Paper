#include "downloader.hpp"

#include <arpa/inet.h>
#include <fmt/format.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <netinet/in.h>  // For sockaddr_in
#include <netinet/in.h>
#include <sys/socket.h>  // For socket functions
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>  // For read

#include <cstdlib>
#include <thread>

namespace {
struct mock_delegate : public downloader_delegate_b {
  mock_delegate() : downloader_delegate_b{nullptr} {}
  MOCK_METHOD(void, download_started, (const std::string &), (override));
  MOCK_METHOD(void, download_ended, (const std::string &), (override));
};

struct Server {
  std::string_view data{};
  size_t port{};
  std::thread s{};
  mutable std::atomic_bool listening{false};
  Server(size_t port, std::string_view data) : data{std::move(data)}, port{std::move(port)} { s = std::thread{&Server::serve, this, data}; }

  ~Server() { s.join(); }
  int serve(const std::string_view data) const {
    const auto sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      fmt::print("Unable to open socket: {}\n", std::strerror(errno));
      return EXIT_FAILURE;
    }

    // Listen on any address
    auto sockaddr = sockaddr_in{};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(port);  // htons is necessary to convert a number to
                                      // network byte order
    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
      fmt::print("Unable to bind to port {}: {}\n", port, std::strerror(errno));
      return EXIT_FAILURE;
    }

    // Start listening. Hold at most 1 connections in the queue
    listening = true;
    if (listen(sockfd, 1) < 0) {
      fmt::print("Listen failed {}\n", std::strerror(errno));
      return EXIT_FAILURE;
    }

    // Grab a connection from the queue
    auto client_addr = sockaddr_in{};
    const auto addrlen = sizeof(client_addr);
    const auto connection = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
    if (connection < 0) {
      fmt::print("Connection failed {}\n", std::strerror(errno));
      return EXIT_FAILURE;
    }

    // Read from the connection
    constexpr auto buffer_size = 100;
    char buffer[buffer_size];
    const auto bytesRead = read(connection, buffer, buffer_size);

    const auto http_message = fmt::format(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: {}\r\n\r\n"
        "{}",
        data.size(), data);
    // Send a message to the connection
    const auto sent = send(connection, http_message.c_str(), http_message.size(), 0);
    if (sent != http_message.size()) {
      fmt::print("Unable to send {}\n", http_message);
      return EXIT_FAILURE;
    }

    // Close the connections
    close(connection);
    close(sockfd);
    return EXIT_SUCCESS;
  }
};
}  // namespace

TEST(DownloaderTest, EarthPornURL) {
  constexpr auto data = "hello";
  constexpr auto port = 50000;
  const auto server = Server{port, data};
  const auto uut = downloader{nullptr};
  while (!server.listening) {
  }
  const auto actual = uut.perform_string(fmt::format("localhost:{}", port));
  ASSERT_EQ(data, actual);
}
