#include "environment_configuration.hpp"

#include <fmt/format.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <optional>

namespace {
struct ensure_env_var_not_set {
  ensure_env_var_not_set(const std::string var) : var{std::move(var)} {
    const auto* from_env = std::getenv(var.c_str());
    if (from_env != nullptr) {
      old_var = std::string{from_env};
    }
    const auto success = unsetenv(var.c_str());
    if (success != 0) {
      throw std::runtime_error(fmt::format("Unable to unset {}: {}", var, strerror(errno)));
    }
  }

  ~ensure_env_var_not_set() {
    if (old_var) {
      setenv(var.c_str(), old_var.value().c_str(), 1);
    }
  }

 private:
  std::string var{};
  std::optional<std::string> old_var{};
};

struct temporary_env_var_set {
  temporary_env_var_set(const std::string var, const std::string value) : var{std::move(var)} {
    const auto* from_env = std::getenv(var.c_str());
    if (from_env != nullptr) {
      old_var = std::string{from_env};
    }
    const auto success = setenv(var.c_str(), value.c_str(), 1);
    if (success != 0) {
      throw std::runtime_error(fmt::format("Unable to set {}: {}", var, strerror(errno)));
    }
  }

  ~temporary_env_var_set() {
    if (old_var) {
      setenv(var.c_str(), old_var.value().c_str(), 1);
    }
  }

 private:
  std::string var{};
  std::optional<std::string> old_var{};
};
}  // namespace

TEST(EnvironmentConfigurationHelper, Unsets) {
  const auto set = setenv("SOMETHING", "ELSE", 1);
  ASSERT_EQ(0, set);
  {
    const auto uut = ensure_env_var_not_set("SOMETHING");
    const auto* from_env = std::getenv("SOMETHING");
    ASSERT_EQ(nullptr, from_env);
  }
  const auto* from_env = std::getenv("SOMETHING");
  ASSERT_STREQ(from_env, "ELSE");
  const auto unset = unsetenv("SOMETHING");
  ASSERT_EQ(0, unset);
}

TEST(EnvironmentConfigurationHelper, Sets) {
  const auto set = setenv("SOMETHING", "ELSE", 1);
  ASSERT_EQ(0, set);
  {
    const auto uut = temporary_env_var_set("SOMETHING", "OTHER");
    const auto* from_env = std::getenv("SOMETHING");
    ASSERT_STREQ(from_env, "OTHER");
  }
  const auto* from_env = std::getenv("SOMETHING");
  ASSERT_STREQ(from_env, "ELSE");
  const auto unset = unsetenv("SOMETHING");
  ASSERT_EQ(0, unset);
}

TEST(EnvironmentConfiguration, GetCacheLocationUsesDefaultWhenNotSet) {
  const auto raii_unset = ensure_env_var_not_set{std::string{environment_configuration::environment_variables::cache_location}};
  constexpr auto variable = environment_configuration::environment_variables::cache_location;
  const auto default_values = default_configuration{}.with_cache_location("HERE");
  const auto uut = environment_configuration{default_values};
  ASSERT_EQ(uut.cache_location().value(), "HERE");
}

TEST(EnvironmentConfiguration, GetCacheLocation) {
  const auto raii_unset = temporary_env_var_set{std::string{environment_configuration::environment_variables::cache_location}, "HELLO"};
  constexpr auto variable = environment_configuration::environment_variables::cache_location;
  const auto default_values = default_configuration{};
  const auto uut = environment_configuration{default_values};
  ASSERT_EQ(uut.cache_location().value(), "HELLO");
}

TEST(EnvironmentConfiguration, GetCacheLocationIgnoreDefault) {
  const auto raii_unset = temporary_env_var_set{std::string{environment_configuration::environment_variables::cache_location}, "HELLO"};
  constexpr auto variable = environment_configuration::environment_variables::cache_location;
  const auto default_values = default_configuration{}.with_cache_location("WORLD");
  const auto uut = environment_configuration{default_values};
  ASSERT_EQ(uut.cache_location().value(), "HELLO");
}

TEST(DefaultEnvironmentConfiguration, CacheLocationHAsSensibleDefault) {
  const auto uut = default_configuration{};
  ASSERT_EQ(uut.cache_location().value(), "/tmp/swear_paper_cache");
}