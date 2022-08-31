#ifndef IMAGE_LOC_HPP
#define IMAGE_LOC_HPP

#include <fmt/format.h>

struct image_location {
    int x{};
    int y{};
};

template <> struct fmt::formatter<image_location> : fmt::formatter<int> {

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext> auto format(const image_location &s, FormatContext &ctx) {
        // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) // c++11
        // ctx.out() is an output iterator to write to.
        return format_to(ctx.out(), "({}, {})", s.x, s.y);
    }
};

#endif
