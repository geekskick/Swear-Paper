#ifndef JSON_PARSE_DEL_B_H
#define JSON_PARSE_DEL_B_H

#include "program_delegate_b.hpp"
#include <string>

class json_parse_delegate_b {
public:
    json_parse_delegate_b(std::shared_ptr<program_delegate_b> &prog_del) : m_del(prog_del) {}
    virtual ~json_parse_delegate_b() {}
    virtual void parse_result(const std::string &value) const = 0;

protected:
    std::shared_ptr<program_delegate_b> &m_del;
};

#endif
