#ifndef JSON_PARSE_DEL_B_H
#define JSON_PARSE_DEL_B_H

#include <string>
#include "program_delegate_b.hpp"

class json_parse_delegate_b {
   public:
    json_parse_delegate_b(std::shared_ptr<program_delegate_b>& prog_del) : m_del(prog_del) {}
    virtual ~json_parse_delegate_b(void) {}
    virtual void raw_json(const std::string& json) = 0;
    virtual void parse_result(const std::string& value) = 0;

   protected:
    std::shared_ptr<program_delegate_b>& m_del;
};

#endif