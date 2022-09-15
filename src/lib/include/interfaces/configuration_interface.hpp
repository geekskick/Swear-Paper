#ifndef SRC_LIB_INCLUDE_INTERFACES_CONFIGURATION_INTERFACE_HPP_
#define SRC_LIB_INCLUDE_INTERFACES_CONFIGURATION_INTERFACE_HPP_

#include <filesystem>
#include <optional>

struct configuration_interface {
  [[nodiscard]] virtual std::optional<std::filesystem::path> cache_location() const = 0;
};

#endif  // SRC_LIB_INCLUDE_INTERFACES_CONFIGURATION_INTERFACE_HPP_
