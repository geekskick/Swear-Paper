#include "program_delegate_b.hpp"

class verbose_program_delegate : public program_delegate_b {
   public:
    virtual ~verbose_program_delegate(void);

    virtual void info(const std::string& msg);
    virtual void error(const std::string& msg);
    virtual void success(const std::string& msg);
};

class quiet_program_delegate : public verbose_program_delegate {
   public:
    virtual void info(const std::string& msg) override;
};