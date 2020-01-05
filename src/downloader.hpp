#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

#include "downloader_delegate_b.hpp"
#include "curl/curl.h"
#include <vector>
#include <optional>

class downloader {
public:
    downloader();
    explicit downloader(std::unique_ptr<downloader_delegate_b> delegate);
    ~downloader();

    std::optional<std::string> perform_string(const std::string &url) const;
    std::optional<std::vector<std::string>> perform_vector(const std::string &url) const;
    std::optional<std::vector<char>> perform_image(const std::string &url) const;

private:
    void *m_curl; 
    std::unique_ptr<downloader_delegate_b> m_del;

    static size_t write_data_to_string(void *ptr, size_t size, size_t nmemb, void *stream);
    static size_t write_data_to_vector(void *ptr, size_t size, size_t nmemb, void *stream);
    void check_rc(const CURLcode &rc, const std::string &msg) const;
};

#endif /* downloader_hpp */
