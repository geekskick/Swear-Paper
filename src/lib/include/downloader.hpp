#ifndef SRC_LIB_INCLUDE_DOWNLOADER_HPP_
#define SRC_LIB_INCLUDE_DOWNLOADER_HPP_

#include <curl/curl.h>
#include <spdlog/spdlog.h>

#include <optional>
#include <string>
#include <vector>

class downloader {
 public:
  downloader();
  ~downloader();

  std::optional<std::string> perform_string(const std::string &url) const;
  std::optional<std::vector<std::string>> perform_vector(const std::string &url) const;
  std::optional<std::vector<char>> perform_image(const std::string &url) const;

 private:
  void *m_curl;

  static size_t write_data_to_string(void *ptr, size_t size, size_t nmemb, void *userdata);
  static size_t write_data_to_vector(void *ptr, size_t size, size_t nmemb, void *stream);
  void check_rc(const CURLcode &rc, const std::string &msg) const;
};

#endif  // SRC_LIB_INCLUDE_DOWNLOADER_HPP_
