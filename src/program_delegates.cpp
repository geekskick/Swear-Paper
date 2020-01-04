#include "program_delegates.hpp"

#include "ansi_codes.hpp"
#include "fmt/format.h"
#include <iostream>

verbose_program_delegate::~verbose_program_delegate(void) {
    fmt::print(ansi_codes::reset);
    fmt::print(stderr, ansi_codes::reset);
}

void verbose_program_delegate::info(const std::string &msg) {
    fmt::print("{} [INFO]\t {} {}\n", ansi_codes::yellow, ansi_codes::reset, msg);
}

void verbose_program_delegate::error(const std::string &msg) {
    fmt::print(stderr, "{} [ERROR]\t {} {}\n", ansi_codes::red, ansi_codes::reset, msg);
}

void verbose_program_delegate::success(const std::string &msg) {
    fmt::print("{} [SUCCESS]\t {} {}\n", ansi_codes::green, ansi_codes::reset, msg);
}

void quiet_program_delegate::info(const std::string &) {}
