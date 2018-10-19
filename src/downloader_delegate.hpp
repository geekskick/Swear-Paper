#ifndef DOWNLOAD_DEL_H
#define DOWNLOAD_DEL_H

#include "downloader_delegate_b.hpp"

class downloader_delegate : public downloader_delegate_b {
   public:
    explicit downloader_delegate(std::shared_ptr<program_delegate_b> &prog_del) : downloader_delegate_b(prog_del) {}

    void download_started(const std::string &url) override;
    void download_ended(const std::string &url) override;
};

#endif