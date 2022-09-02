#ifndef IMAGE_SIZE_HPP
#define IMAGE_SIZE_HPP

#include <fmt/format.h>

template <typename SizeType>
struct image_size {
  SizeType w{};
  SizeType h{};
};

template <typename SizeType>
struct fmt::formatter<image_size<SizeType>> : fmt::formatter<double> {
  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const image_size<SizeType> &s, FormatContext &ctx) {
    // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) //
    // c++11 ctx.out() is an output iterator to write to.
    return format_to(ctx.out(), "({}, {})", s.w, s.h);
  }
};

#endif
