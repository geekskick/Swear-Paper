#ifndef SRC_LIB_INCLUDE_ENVIRONMENT_CONFIGURATION_HPP_
#define SRC_LIB_INCLUDE_ENVIRONMENT_CONFIGURATION_HPP_
#include <string_view>

#include "interfaces/configuration_interface.hpp"

class default_configuration : public configuration_interface {
 public:
  [[nodiscard]] std::optional<std::filesystem::path> cache_location() const override;
  [[nodiscard]] default_configuration& with_cache_location(const std::filesystem::path& path);

 private:
  std::optional<std::filesystem::path> cache_location_{std::filesystem::temp_directory_path() / "swear_paper_cache"};
};

class environment_configuration : public configuration_interface {
 public:
  struct environment_variables {
    constexpr static std::string_view cache_location{"SWEAR_PAPER_CACHE"};
  };

  explicit environment_configuration(const configuration_interface& default_values) : default_values{default_values} {}
  [[nodiscard]] std::optional<std::filesystem::path> cache_location() const override;

 private:
  const configuration_interface& default_values;
};

#endif  // SRC_LIB_INCLUDE_ENVIRONMENT_CONFIGURATION_HPP_
