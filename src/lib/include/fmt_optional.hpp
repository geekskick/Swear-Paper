#ifndef SRC_LIB_INCLUDE_FMT_OPTIONAL_HPP_
#define SRC_LIB_INCLUDE_FMT_OPTIONAL_HPP_

#include <fmt/format.h>

#include <filesystem>
#include <optional>
template <typename T>
struct fmt::formatter<std::optional<T>> : fmt::formatter<T> {
  template <typename FormatContext>
  auto format(const std::optional<T>& opt, FormatContext& ctx) {
    if (opt) {
      fmt::formatter<T>::format(*opt, ctx);
      return ctx.out();
    }
    return fmt::format_to(ctx.out(), "NO VALUE");
  }
};

template <>
struct fmt::formatter<std::filesystem::path> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
    const auto* const begin = ctx.begin();
    const auto* const end = ctx.end();

    // Check if reached the end of the range:
    if (begin != end && *begin != '}') throw format_error("invalid format");

    return begin;
  }

  template <typename FormatContext>
  auto format(const std::filesystem::path& path, FormatContext& ctx) {
    return fmt::format_to(ctx.out(), "{}", path.c_str());
  }
};
#endif  // SRC_LIB_INCLUDE_FMT_OPTIONAL_HPP_
