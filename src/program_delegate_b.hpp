#ifndef PROGRAM_DEL_B_HPP
#define PROGRAM_DEL_B_HPP

#include <string>

class program_delegate_b {
public:
    virtual ~program_delegate_b() = default;
    virtual void info(const std::string &msg) = 0;
    virtual void error(const std::string &msg) = 0;
    virtual void success(const std::string &msg) = 0;
};
#endif
