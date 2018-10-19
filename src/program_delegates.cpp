#include "program_delegates.hpp"

#include <iostream>
#include "ansi_codes.hpp"

verbose_program_delegate::~verbose_program_delegate(void) {
    std::cout << ansi_codes::reset;
    std::cerr << ansi_codes::reset;
}

void verbose_program_delegate::info(const std::string& msg) {
    std::cout << ansi_codes::yellow << "[INFO]\t" << ansi_codes::reset << msg << std::endl;
}

void verbose_program_delegate::error(const std::string& msg) { std::cerr << ansi_codes::red << "[ERROR]\t" << ansi_codes::reset << msg << std::endl; }

void verbose_program_delegate::success(const std::string& msg) {
    std::cout << ansi_codes::green << "[SUCCESS]\t" << ansi_codes::reset << msg << std::endl;
}

void quiet_program_delegate::info(const std::string&) {}
