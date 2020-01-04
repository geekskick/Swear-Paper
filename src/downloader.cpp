//
//  downloader.cpp
//  Swear Paper
//

#include "downloader.hpp"
#include <curl/easy.h>
#include <sstream>

// constructor inits libcurl

downloader::downloader(std::shared_ptr<downloader_delegate_b> delegate) : m_curl(curl_easy_init()), m_del(delegate) {}
downloader::downloader() : m_curl(curl_easy_init()) {}

// dtor safely cleans up the downloader
downloader::~downloader() { curl_easy_cleanup(m_curl); }

// Callback for putting the data rx'd into a string
size_t downloader::write_data_to_string(void *ptr, size_t size, size_t nmemb, void *stream) {
    std::string data(static_cast<const char *>(ptr),
                     static_cast<size_t>(size * nmemb)); // create new string from the data
    *static_cast<std::string*>(stream) += data;       // add the data to the out string
    return size * nmemb;
}

// Do the download and get the reply as a string
std::optional<std::string> downloader::perform_string(const std::string &url) const {
    CURLcode code; // error codes

    auto result = std::string{};

    // connect to the url
    code = curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    check_rc(code, "curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str()");

    // when reply is recieved from the write function it goes to this call back
    code = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string);
    check_rc(code, "url_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string)");

    // argument to send to the write callback
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

    long response_code = {0};
    const long success = {200};
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

    std::string to;                             // used for iteration
    std::stringstream stream(*reply_as_string); // The big reply as a stringstream
    std::vector<std::string> words;             // the final vector

    // iterate over the big string and put the lines into the 'to' variable, then
    // push that to the back of the vector
    while (std::getline(stream, to, '\n')) {
        words.push_back(to);
    }

    return words;
}

// Callback for putting the data rx'd into a vector of chars
size_t downloader::write_data_to_vector(void *inptr, size_t size, size_t nmemb, void *userdata) {
    char *ptr = static_cast<char *>(inptr);
    std::vector<char> *stream{static_cast<std::vector<char> *>(userdata)};
    size_t count{size * nmemb};
    stream->insert(stream->end(), ptr, ptr + count);
    return count;
}

// Do the download and get the reply as a vector of charactrers (image)!
std::optional<std::vector<char>> downloader::perform_image(const std::string &url) const {
    CURLcode code;

    // connect to the url
    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

    // when reply is recieved from the write function it goes to this call back
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_vector);

    auto result = std::vector<char>{};
    // argument to send to the write callback
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);

    // do it!
    if (m_del) {
        m_del->download_started(url);
    }

    code = curl_easy_perform(m_curl);

    if (m_del) {
        m_del->download_ended(url);
    }

    if(code == CURLE_OK){
       return {}; 
    }

    return result;
}
