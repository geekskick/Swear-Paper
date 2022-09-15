
#ifndef SRC_LIB_INCLUDE_CACHE_OR_DOWNLOAD_HPP_
#define SRC_LIB_INCLUDE_CACHE_OR_DOWNLOAD_HPP_

#include <filesystem>
#include <fstream>
#include <string_view>

#include "interfaces/downloader_interface.hpp"

class CacheOrDownloadStrings {
  std::filesystem::path cache_location{};
  std::string_view download_url{};
  const downloader_interface& d;
  std::optional<std::vector<std::string>> value{};
  bool value_set{false};
  void create_cache(const std::vector<std::string>& to_write);
  static std::vector<std::string> read_vector_from_file(const std::filesystem::path& filename);

 public:
  CacheOrDownloadStrings(std::filesystem::path location, std::string_view url, const downloader_interface& download)
      : cache_location{std::move(location)}, download_url{std::move(url)}, d{download} {}

  [[nodiscard]] std::optional<std::vector<std::string>>& get();
  [[nodiscard]] operator bool() const;
};

#endif