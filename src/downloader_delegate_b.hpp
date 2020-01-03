#ifndef DOWNLOADER_DEL_B_H
#define DOWNLOADER_DEL_B_H

#include "program_delegate_b.hpp"
#include <memory>
#include <string>

class downloader_delegate_b {
public:
    virtual ~downloader_delegate_b(void) {}
    downloader_delegate_b(std::shared_ptr<program_delegate_b> &prog_del) : m_del(prog_del) {}

    virtual void download_started(const std::string &url) = 0;
    virtual void download_ended(const std::string &url) = 0;

protected:
    std::shared_ptr<program_delegate_b> &m_del;
};

#endif