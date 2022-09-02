#ifndef PROGRAM_DELEGATES_HPP
#define PROGRAM_DELEGATES_HPP
#include "program_delegate_b.hpp"

class verbose_program_delegate : public program_delegate_b {
 public:
  virtual ~verbose_program_delegate() = default;
  void info(const std::string &msg) const override;
  void error(const std::string &msg) const override;
  void success(const std::string &msg) const override;
};

class quiet_program_delegate : public verbose_program_delegate {
 public:
  void info(const std::string &msg) const override;
};
#endif
