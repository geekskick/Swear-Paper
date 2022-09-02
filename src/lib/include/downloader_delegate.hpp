#ifndef DOWNLOAD_DEL_HPP
#define DOWNLOAD_DEL_HPP

#include <memory>  // for shared_ptr
#include <string>  // for string

#include "interfaces/downloader_delegate_b.hpp"
class program_delegate_b;

class downloader_delegate : public downloader_delegate_b {
 public:
  explicit downloader_delegate(std::shared_ptr<program_delegate_b> &prog_del) : downloader_delegate_b(prog_del) {}

  void download_started(const std::string &url) override;
  void download_ended(const std::string &url) override;
};

#endif
