#include "json_parse_delegate.hpp"
#include <iostream>

void json_parse_delegate::raw_json(const std::string &json) { m_del->info("Raw json is " + json); }

void json_parse_delegate::parse_result(const std::string &value) { m_del->info("Value extracted from json is " + value); }