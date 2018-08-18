#ifndef JSON_PARSE_DEL_B_H
#define JSON_PARSE_DEL_B_H

#include <string>

class json_parse_delegate_b {
public:
  virtual ~json_parse_delegate_b(void) {}
  virtual void raw_json(const std::string &json) = 0;
  virtual void parse_result(const std::string &value) = 0;
};

#endif