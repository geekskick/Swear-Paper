#include "program_delegate_b.hpp"

class verbose_program_delegate : public program_delegate_b {
public:
    virtual ~verbose_program_delegate();

    void info(const std::string &msg) override;
    void error(const std::string &msg) override;
    void success(const std::string &msg) override;
};

class quiet_program_delegate : public verbose_program_delegate {
public:
    void info(const std::string &msg) override;
};
