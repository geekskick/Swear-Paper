#include "environment_configuration.hpp"

#include <spdlog/spdlog.h>

#include <boost/lexical_cast.hpp>

#include "fmt_optional.hpp"

namespace {

std::optional<std::string> get_string_from_env(const std::string& variable) {
  const auto* set_in_env = getenv(variable.c_str());
  spdlog::trace("Got from environment {}:{}", variable, set_in_env ? set_in_env : "");
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
    spdlog::error("Unable to get {} from env: ", variable, e.what());
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
  if (from_env.has_value()) {
    spdlog::trace("Get a value from the environment, so using that");
    return from_env;
  } else {
    spdlog::trace("Failed to get a value from the environment, so using default value {}", default_values.cache_location());
    return default_values.cache_location();
  }
};
std::optional<std::filesystem::path> default_configuration::cache_location() const { return cache_location_; }
default_configuration& default_configuration::with_cache_location(std::filesystem::path path) {
  cache_location_ = path;
  return *this;
}
