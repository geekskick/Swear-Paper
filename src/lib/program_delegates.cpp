#include "program_delegates.hpp"

#include <fmt/color.h>
#include <fmt/format.h>

void verbose_program_delegate::info(const std::string &msg) const {
  fmt::print(fmt::fg(fmt::color::blue), "[INFO]\t");
  fmt::print("{}\n", msg);
}

void verbose_program_delegate::error(const std::string &msg) const {
  fmt::print(stderr, fmt::fg(fmt::color::red), "[ERROR]\t");
  fmt::print(stderr, "{}\n", msg);
}

void verbose_program_delegate::success(const std::string &msg) const {
  fmt::print(fmt::fg(fmt::color::green), "[SUCCESS]\t");
  fmt::print("{}\n", msg);
}

void quiet_program_delegate::info(const std::string &) const {}
