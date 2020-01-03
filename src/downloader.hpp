//
//  downloader.hpp
//  Swear Paper
//

#ifndef downloader_hpp
#define downloader_hpp

#include "downloader_delegate_b.hpp"
#include "curl/curl.h"
#include <iostream>
#include <vector>
#include <optional>

class downloader {
public:
    downloader();
    explicit downloader(std::shared_ptr<downloader_delegate_b> delegate);
    ~downloader();

    std::optional<std::string> perform_string(const std::string &url);
    std::optional<std::vector<std::string>> perform_vector(const std::string &url);
    std::pair<bool, std::string> perform_image(const std::string &url, std::vector<char> &result);

private:
    void *m_curl; // curl object
    std::shared_ptr<downloader_delegate_b> m_del;

    // when data is recieved this function puts it into the stream
    static size_t write_data_to_string(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t write_data_to_vector(void *ptr, size_t size, size_t nmemb, void *stream);
    void check_rc(const CURLcode &rc, const std::string &msg) const;
};

#endif /* downloader_hpp */
