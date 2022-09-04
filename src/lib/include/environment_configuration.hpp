#ifndef ENV_CONFIG_HPP
#define ENV_CONFIG_HPP
#include <string_view>

#include "interfaces/configuration_interface.hpp"

class default_configuration : public configuration_interface {
 public:
  [[nodiscard]] std::optional<std::filesystem::path> cache_location() const override;
  [[nodiscard]] default_configuration& with_cache_location(std::filesystem::path path);

 private:
  std::optional<std::filesystem::path> cache_location_{std::filesystem::temp_directory_path() / "swear_paper_cache"};
};

class environment_configuration : public configuration_interface {
 public:
  struct environment_variables {
    constexpr static std::string_view cache_location{"SWEAR_PAPER_CACHE"};
  };

  environment_configuration(const configuration_interface& default_values) : default_values{default_values} {}
  [[nodiscard]] std::optional<std::filesystem::path> cache_location() const override;

 private:
  const configuration_interface& default_values;
};

#endif