#include "json_parse_delegate.hpp"
#include <iostream>

void json_parse_delegate::raw_json(const std::string& json) {
  std::cout << "Raw json is " << json << std::endl;
}

void json_parse_delegate::parse_result(const std::string& value) {
  std::cout << "Value extracted is " << value << std::endl;
}