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

struct HTTPOneShotServer {
  struct HTTPOptions {
    std::string_view http_return_code{"200 OK"};
    std::string_view http_version{"1.1"};
    // https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
    constexpr HTTPOptions();
    constexpr HTTPOptions &with_return_code(std::string_view code) {
      http_return_code = code;
      return *this;
    }
    constexpr HTTPOptions &with_version(std::string_view version) {
      http_version = version;
      return *this;
    }
  };
  [[nodiscard]] bool is_listening() const { return listening; }
  HTTPOneShotServer(size_t port, std::string_view data, HTTPOptions options = HTTPOneShotServer::HTTPOptions())
      : data{std::move(data)}, port{std::move(port)}, s{std::thread{&HTTPOneShotServer::serve, this, data}}, options{std::move(options)} {}
  ~HTTPOneShotServer() { s.join(); }

 private:
  std::string_view data{};
  size_t port{};
  std::thread s{};
  mutable std::atomic_bool listening{false};
  HTTPOptions options{};

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
    const auto bytes_read = read(connection, buffer, buffer_size);

    const auto http_message = fmt::format(
        "HTTP/{} {}\r\n"
        "Content-Length: {}\r\n\r\n"
        "{}",
        options.http_version, options.http_return_code, data.size(), data);
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

// https://stackoverflow.com/questions/53408962/try-to-understand-compiler-error-message-default-member-initializer-required-be
constexpr HTTPOneShotServer::HTTPOptions::HTTPOptions() = default;
}  // namespace

TEST(DownloaderTest, CanDownloadHTTP) {
  constexpr auto data = "hello";
  constexpr auto port = 50000;
  const auto server = HTTPOneShotServer{port, data};
  const auto uut = downloader{nullptr};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_string(fmt::format("localhost:{}", port));
  ASSERT_EQ(data, actual);
}

TEST(DownloaderTest, NullOptIfNot200) {
  constexpr auto data = "hello";
  constexpr auto port = 50001;
  constexpr auto options = HTTPOneShotServer::HTTPOptions().with_return_code("404 Not Found");
  const auto server = HTTPOneShotServer{port, data, options};
  const auto uut = downloader{nullptr};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_string(fmt::format("localhost:{}", port));
  ASSERT_EQ(actual, std::nullopt);
}
