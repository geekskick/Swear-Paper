//
//  downloader.cpp
//  Swear Paper
//

#include "downloader.hpp"
#include <curl/easy.h>
#include <sstream>

// constructor inits libcurl

downloader::downloader(std::unique_ptr<downloader_delegate_b> delegate)
    : m_curl{curl_easy_init()}, m_del{std::move(delegate)} {}
downloader::downloader() : m_curl(curl_easy_init()) {}

// dtor safely cleans up the downloader
downloader::~downloader() { curl_easy_cleanup(m_curl); }

// Callback for putting the data rx'd into a string
size_t downloader::write_data_to_string(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data(static_cast<const char *>(ptr),
                     static_cast<size_t>(size * nmemb)); // create new string from the data
    *static_cast<std::string *>(stream) += data;         // add the data to the out string
    return size * nmemb;
}

// Do the download and get the reply as a string
std::optional<std::string> downloader::perform_string(const std::string &url) const {
    auto code = CURLcode{};

    code = curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    check_rc(code, "curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str()");

    code = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string);
    check_rc(code, "url_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string)");

    auto result = std::string{};
    code = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);
    check_rc(code, "url_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result)");

    // do it!
    if (m_del) {
        m_del->download_started(url);
    }

    code = curl_easy_perform(m_curl);

    if (m_del) {
        m_del->download_ended(url);
    }

    check_rc(code, "url_easy_perform(m_curl)");

    int64_t response_code = {0};
    const int64_t success = {200};
    code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
    check_rc(code, "curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code)");

    if (success != response_code) {
        return {};
    }

    return result;
}

void downloader::check_rc(const CURLcode &rc, const std::string &msg) const {
    if (CURLE_OK != rc) {
        throw std::runtime_error(std::string(msg + std::string(": ") + std::string(curl_easy_strerror(rc))));
    }
}

// Do the download and get the reply as a vector of strings
std::optional<std::vector<std::string>> downloader::perform_vector(const std::string &url) const {
    const auto reply_as_string{perform_string(url)};
    if (!reply_as_string) {
        return {};
    };

    auto to = std::string{};
    auto stream = std::stringstream{*reply_as_string};
    auto words = std::vector<std::string>{};

    while (std::getline(stream, to, '\n')) {
        words.push_back(to);
    }

    return words;
}

// Callback for putting the data rx'd into a vector of chars
size_t downloader::write_data_to_vector(void *inptr, size_t size, size_t nmemb, void *userdata) {
    const auto *ptr = static_cast<char *>(inptr);
    const auto count = size * nmemb;

    auto *stream = static_cast<std::vector<char> *>(userdata);
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

// Do the download and get the reply as a vector of charactrers (image)!
std::optional<std::vector<char>> downloader::perform_image(const std::string &url) const {
    auto code = CURLcode{};

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_vector);

    auto result = std::vector<char>{};
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);

    // do it!
    if (m_del) {
        m_del->download_started(url);
    }

    code = curl_easy_perform(m_curl);

    if (m_del) {
        m_del->download_ended(url);
    }
    check_rc(code, "curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code)");

    return result;
}
