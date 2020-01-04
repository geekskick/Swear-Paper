#ifndef ANSI_CODES_H
#define ANSI_CODES_H

#include <string_view>

struct ansi_codes {
    constexpr static std::string_view red {"\u001b[31m"};
    constexpr static std::string_view reset{"\u001b[0m"};
    constexpr static std::string_view yellow{"\u001b[33m"};
    constexpr static std::string_view green{"\u001b[32m"};
};

#endif
