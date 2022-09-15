#ifndef SRC_LIB_INCLUDE_INTERFACES_DOWNLOADER_INTERFACE_HPP_
#define SRC_LIB_INCLUDE_INTERFACES_DOWNLOADER_INTERFACE_HPP_

#include <optional>
#include <string>
#include <vector>
struct downloader_interface {
  virtual ~downloader_interface() = default;

  virtual std::optional<std::string> perform_string(const std::string &url) const = 0;
  virtual std::optional<std::vector<std::string>> perform_vector(const std::string &url) const = 0;
  virtual std::optional<std::vector<char>> perform_image(const std::string &url) const = 0;
};

#endif  // SRC_LIB_INCLUDE_INTERFACES_DOWNLOADER_INTERFACE_HPP_