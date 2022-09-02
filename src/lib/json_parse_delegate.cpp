#include "include/json_parse_delegate.hpp"

#include "interfaces/program_delegate_b.hpp"  // for program_delegate_b

void json_parse_delegate::parse_result(const std::string &value) const { m_del->info("Value extracted from json is " + value); }
