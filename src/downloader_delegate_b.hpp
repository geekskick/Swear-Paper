#ifndef DOWNLOADER_DEL_B_H
#define DOWNLOADER_DEL_B_H

#include <string>

class downloader_delegate_b {
   public:
    virtual ~downloader_delegate_b(void) {}

    virtual void download_started(const std::string &url) = 0;
    virtual void download_ended(const std::string &url) = 0;
};

#endif