#include "cache_or_download.hpp"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>
#include <string_view>

#include "fmt_optional.hpp"
#include "interfaces/downloader_interface.hpp"

void CacheOrDownloadStrings::create_cache(const std::vector<std::string>& to_write) {
  spdlog::trace("Creating a cached value file: ", cache_location);
  auto file = std::ofstream{cache_location};
  for (const auto& word : to_write) {
    file << word << "\n";
  }
}

std::vector<std::string> CacheOrDownloadStrings::read_vector_from_file(const std::filesystem::path& filename) {
  auto file = std::ifstream{filename};
  auto rc = std::vector<std::string>{};
  auto line = std::string{};
  while (std::getline(file, line)) {
    rc.emplace_back(std::move(line));
  }
  return rc;
}

std::optional<std::vector<std::string>>& CacheOrDownloadStrings::get() {
  if (value_set) {
    spdlog::trace("Value already set, so not attempting to get again");
    return value;
  }
  const auto cache_exists = std::filesystem::exists(cache_location);
  if (cache_exists) {
    spdlog::trace("Cache found at {}, so using the contents of that", cache_location);
    value = read_vector_from_file(cache_location);
    value_set = true;
    return value;
  }

  spdlog::debug("Getting value from {}", download_url);
  const auto downloaded_value = [&]() {
    try {
      return d.perform_vector(std::string{download_url});
    } catch (const std::exception& e) {
      return std::optional<std::vector<std::string>>{};
    }
  }();

  if (!downloaded_value) {
    spdlog::error("Unable to get value from {}", download_url);
    return value;
  }

  create_cache(downloaded_value.value());

  spdlog::trace("Providing value");
  value = std::move(downloaded_value);
  value_set = true;
  return value;
}

CacheOrDownloadStrings::operator bool() const { return value.has_value(); }
