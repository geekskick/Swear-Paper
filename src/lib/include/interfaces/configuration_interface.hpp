#ifndef CONFIG_INTERFACE_HPP
#define CONFIG_INTERFACE_HPP

#include <filesystem>
#include <optional>

struct configuration_interface {
  [[nodiscard]] virtual std::optional<std::filesystem::path> cache_location() const = 0;
};

#endif