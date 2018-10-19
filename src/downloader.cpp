//
//  downloader.cpp
//  Swear Paper
//

#include "downloader.hpp"
#include <curl/easy.h>
#include <sstream>

size_t curl_downloader::write_data_to_string(void* ptr, size_t size, size_t nmemb, void* stream) {
    std::string data((const char*)ptr,
                     (size_t)size * nmemb);  // create new string from the data
    *((std::string*)stream) += data;         // add the data to the out string
    return size * nmemb;
}

void curl_downloader::check_rc(const CURLcode& rc, const std::string& msg) const {
    if (CURLE_OK != rc) {
        throw std::runtime_error(std::string(msg + std::string(": ") + std::string(curl_easy_strerror(rc))));
    }
}

std::vector<std::string> curl_downloader::get_vec(const std::string& from) const {
     std::string reply_as_string { get_str(from) };  // the big string

     std::string to;                  // used for iteration
     std::stringstream stream(reply_as_string);   // The big reply as a stringstream
     std::vector<std::string> words;  // the final vector

     // iterate over the big string and put the lines into the 'to' variable, then
     // push that to the back of the vector
     while (std::getline(stream, to, '\n')) {
         words.push_back(to);
     }

     return words;
}
std::vector<char> curl_downloader::get_img(const std::string& from) const {
    (void)from;
    return std::vector<char>();
    ;
};

std::string curl_downloader::get_str(const std::string& from) const {
    CURLcode code;  // error codes
    std::string result{""};
    // connect to the url
    code = curl_easy_setopt(m_curl, CURLOPT_URL, from.c_str());
    check_rc(code, "curl_easy_setopt(m_curl, CURLOPT_URL, " + from + ")");

    // when reply is recieved from the write function it goes to this call back
    code = curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string);
    check_rc(code, "url_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string)");

    // argument to send to the write callback
    code = curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);
    check_rc(code, "url_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result)");

    if (m_del) {
        m_del->download_started(from);
    }
    code = curl_easy_perform(m_curl);
    if (m_del) {
        m_del->download_ended(from);
    }
    check_rc(code, "url_easy_perform(m_curl)");

    long response_code = {0};
    const long success = {200};
    code = curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code);
    check_rc(code, "curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &response_code)");

    if (success != response_code) {
        throw std::runtime_error("response_code");
    }

    return result;
}

// // Do the download and get the reply as a vector of strings
// std::pair<bool, std::string> downloader::perform_vector(const std::string &url, std::vector<std::string> &result) {
//     std::string str;
//     auto reply_as_string{perform_string(url, str)};  // the big string
//     if (!reply_as_string.first) {
//         return reply_as_string;
//     };

//     std::string to;                  // used for iterationg
//     std::stringstream stream(str);   // The big reply as a stringstream
//     std::vector<std::string> words;  // the final vector

//     // iterate over the big string and put the lines into the 'to' variable, then
//     // push that to the back of the vector
//     while (std::getline(stream, to, '\n')) {
//         words.push_back(to);
//     }

//     result = words;

//     return reply_as_string;
// }

// // Callback for putting the data rx'd into a vector of chars
// size_t downloader::write_data_to_vector(void *inptr, size_t size, size_t nmemb, void *userdata) {
//     char *ptr = (char *)inptr;
//     std::vector<char> *stream{(std::vector<char> *)userdata};
//     size_t count{size * nmemb};
//     stream->insert(stream->end(), ptr, ptr + count);
//     return count;
// }

// // Do the download and get the reply as a vector of charactrers (image)!
// std::pair<bool, std::string> downloader::perform_image(const std::string &url, std::vector<char> &result) {
//     CURLcode code;

//     // connect to the url
//     curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

//     // when reply is recieved from the write function it goes to this call back
//     curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_vector);

//     // argument to send to the write callback
//     curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);

//     // do it!
//     if (m_del) {
//         m_del->download_started(url);
//     }

//     code = curl_easy_perform(m_curl);

//     if (m_del) {
//         m_del->download_ended(url);
//     }

//     return {code == CURLE_OK, curl_easy_strerror(code)};
// }