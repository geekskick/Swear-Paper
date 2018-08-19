#ifndef JSON_PARSE_DEL_H
#define JSON_PARSE_DEL_H

#include <string>
#include "json_parse_delegate_b.hpp"

class json_parse_delegate : public json_parse_delegate_b {
   public:
    explicit json_parse_delegate(std::shared_ptr<program_delegate_b> &prog_del) : json_parse_delegate_b(prog_del) {}

    void parse_result(const std::string &value) const override;
};

#endif