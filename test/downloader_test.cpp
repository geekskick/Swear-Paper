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
  HTTPOneShotServer(std::string_view data, HTTPOptions options = HTTPOneShotServer::HTTPOptions())
      : data{std::move(data)}, options{std::move(options)}, s{std::thread{&HTTPOneShotServer::serve, this, data}} {}
  ~HTTPOneShotServer() { s.join(); }

  size_t listening_port() const { return next_attempted_port - 1; }

 private:
  std::string_view data{};
  mutable std::atomic_size_t next_attempted_port{5000};
  HTTPOptions options{};
  std::thread s{};
  mutable std::atomic_bool listening{false};

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

    do {
      sockaddr.sin_port = htons(next_attempted_port++);  // htons is necessary to convert a number to
      fmt::print("Trying to bind to port {}\n", next_attempted_port - 1);
      // network byte order
    } while (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0);

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
    read(connection, buffer, buffer_size);

    const auto http_message = fmt::format(
        "HTTP/{} {}\r\n"
        "Content-Length: {}\r\n\r\n"
        "{}",
        options.http_version, options.http_return_code, data.size(), data);
    // Send a message to the connection
    const auto sent = static_cast<size_t>(send(connection, http_message.c_str(), http_message.size(), 0));
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

TEST(DownloaderTest, CanDownloadHTTPString) {
  constexpr auto data = "hello";
  const auto server = HTTPOneShotServer{data};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_string(fmt::format("localhost:{}", server.listening_port()));
  ASSERT_EQ(data, actual);
}

TEST(DownloaderTest, NullOptIfNot200String) {
  constexpr auto data = "hello";
  constexpr auto options = HTTPOneShotServer::HTTPOptions().with_return_code("404 Not Found");
  const auto server = HTTPOneShotServer{data, options};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_string(fmt::format("localhost:{}", server.listening_port()));
  ASSERT_EQ(actual, std::nullopt);
}

TEST(DownloaderTest, ThrowsIfNotHTTP11String) {
  constexpr auto data = "hello";
  constexpr auto options = HTTPOneShotServer::HTTPOptions().with_version("0.9");
  const auto server = HTTPOneShotServer{data, options};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  ASSERT_ANY_THROW(uut.perform_string(fmt::format("localhost:{}", server.listening_port())));
}

TEST(DownloaderTest, CanDownloadHTTPStringVector) {
  constexpr auto data = "hello\nworld";
  const auto server = HTTPOneShotServer{data};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_vector(fmt::format("localhost:{}", server.listening_port()));
  const auto expected = std::vector<std::string>{"hello", "world"};
  ASSERT_THAT(expected, testing::ContainerEq(actual.value()));
}

TEST(DownloaderTest, CanDownloadHTTPStringVectorWithOneItem) {
  constexpr auto data = "hello";
  const auto server = HTTPOneShotServer{data};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_vector(fmt::format("localhost:{}", server.listening_port()));
  const auto expected = std::vector<std::string>{"hello"};
  ASSERT_THAT(expected, testing::ContainerEq(actual.value()));
}

TEST(DownloaderTest, NullOptIfNot200StringVector) {
  constexpr auto data = "hello\nworld";
  constexpr auto options = HTTPOneShotServer::HTTPOptions().with_return_code("404 Not Found");
  const auto server = HTTPOneShotServer{data, options};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_vector(fmt::format("localhost:{}", server.listening_port()));
  ASSERT_EQ(actual, std::nullopt);
}

TEST(DownloaderTest, ThrowsIfNotHTTP11StringVector) {
  constexpr auto data = "hello\nworld";
  constexpr auto options = HTTPOneShotServer::HTTPOptions().with_version("0.9");
  const auto server = HTTPOneShotServer{data, options};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  ASSERT_ANY_THROW(uut.perform_vector(fmt::format("localhost:{}", server.listening_port())));
}

TEST(DownloaderTest, CanDownloadHTTPImage) {
  constexpr auto data = "\x01\x02";
  const auto server = HTTPOneShotServer{data};
  const auto uut = downloader{};
  while (!server.is_listening()) {
  }
  const auto actual = uut.perform_image(fmt::format("localhost:{}", server.listening_port()));

  const auto expected = std::vector<char>{1, 2};
  ASSERT_THAT(expected, testing::ContainerEq(actual.value()));
}
