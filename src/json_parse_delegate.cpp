#include "json_parse_delegate.hpp"
#include <iostream>

void json_parse_delegate::parse_result(const std::string &value) const { m_del->info("Value extracted from json is " + value); }