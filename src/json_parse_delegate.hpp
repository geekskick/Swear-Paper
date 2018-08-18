#ifndef JSON_PARSE_DEL_H
#define JSON_PARSE_DEL_H

#include <string>
#include "json_parse_delegate_b.hpp"

class json_parse_delegate : public json_parse_delegate_b {
  void raw_json(const std::string& json) override;
  void parse_result(const std::string& value) override;
};

#endif