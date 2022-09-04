#include "environment_configuration.hpp"

#include <fmt/core.h>

#include <boost/lexical_cast.hpp>
namespace {

std::optional<std::string> get_string_from_env(const std::string& variable) {
  const auto* set_in_env = getenv(variable.c_str());
  if (set_in_env == nullptr) {
    return {};
  }
  return set_in_env;
}

template <typename T>
std::enable_if_t<!std::is_convertible_v<std::string, T>, std::optional<T>> get_from_env(const std::string& variable) {
  const auto str = get_string_from_env(variable);
  try {
    return boost::lexical_cast<T>(str);
  } catch (boost::bad_lexical_cast& e) {
    fmt::print("Unable to get {} from env: ", variable, e.what());
    return {};
  }
};

template <typename T>
std::enable_if_t<std::is_convertible_v<std::string, T>, std::optional<T>> get_from_env(const std::string& variable) {
  return get_string_from_env(variable);
};

}  // namespace
std::optional<std::filesystem::path> environment_configuration::cache_location() const {
  const auto from_env = get_from_env<std::filesystem::path>(std::string{environment_variables::cache_location});
  return from_env.has_value() ? from_env : default_values.cache_location();
};
std::optional<std::filesystem::path> default_configuration::cache_location() const { return cache_location_; }
default_configuration& default_configuration::with_cache_location(std::filesystem::path path) {
  cache_location_ = path;
  return *this;
}
