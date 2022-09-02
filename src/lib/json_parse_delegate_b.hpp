#ifndef JSON_PARSE_DEL_B_HPP
#define JSON_PARSE_DEL_B_HPP

#include <memory>
#include <string>

#include "program_delegate_b.hpp"

class json_parse_delegate_b {
 public:
  json_parse_delegate_b(std::shared_ptr<program_delegate_b> prog_del)
      : m_del{std::move(prog_del)} {}
  virtual ~json_parse_delegate_b() = default;
  virtual void parse_result(const std::string &value) const = 0;

 protected:
  std::shared_ptr<program_delegate_b> m_del;
};

#endif
