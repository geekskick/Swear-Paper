//
//  downloader.cpp
//  Swear Paper
//

#include "downloader.hpp"
#include <curl/curl.h>
#include <curl/easy.h>
#include <sstream>

// constructor inits libcurl
downloader::downloader() { m_curl = curl_easy_init(); }

// dtor safely cleans up the downloader
downloader::~downloader() { curl_easy_cleanup(m_curl); }

// Callback for putting the data rx'd into a string
size_t downloader::write_data_to_string(void *ptr, size_t size, size_t nmemb,
                                        void *stream) {
  std::string data((const char *)ptr,
                   (size_t)size * nmemb);  // create new string from the data
  *((std::string *)stream) += data;        // add the data to the out string
  return size * nmemb;
}

// Do the download and get the reply as a string
std::pair<bool, std::string> downloader::perform_string(const std::string &url,
                                                        std::string &result) {
  CURLcode code;  // error codes

  // connect to the url
  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

  // when reply is recieved from the write function it goes to this call back
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_string);

  // argument to send to the write callback
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);

  // do it!
  code = curl_easy_perform(m_curl);

  // check the error code
  if (code != CURLE_OK) {
    return {false, curl_easy_strerror(code)};
  }

  return {true, "success"};
}

// Do the download and get the reply as a vector of strings
std::pair<bool, std::string> downloader::perform_vector(
    const std::string &url, std::vector<std::string> &result) {
  std::string str;
  auto reply_as_string{perform_string(url, str)};  // the big string
  if (!reply_as_string.first) {
    return reply_as_string;
  };

  std::string to;                  // used for iterationg
  std::stringstream stream(str);   // The big reply as a stringstream
  std::vector<std::string> words;  // the final vector

  // iterate over the big string and put the lines into the 'to' variable, then
  // push that to the back of the vector
  while (std::getline(stream, to, '\n')) {
    words.push_back(to);
  }

  result = words;

  return {true, "success"};
}

// Callback for putting the data rx'd into a vector of chars
size_t downloader::write_data_to_vector(void *inptr, size_t size, size_t nmemb,
                                        void *userdata) {
  char *ptr = (char *)inptr;
  std::vector<char> *stream{(std::vector<char> *)userdata};
  size_t count = size * nmemb;
  stream->insert(stream->end(), ptr, ptr + count);
  return count;
}

// Do the download and get the reply as a vector of charactrers (image)!
std::pair<bool, std::string> downloader::perform_image(
    const std::string &url, std::vector<char> &result) {
  CURLcode code;

  // connect to the url
  curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());

  // when reply is recieved from the write function it goes to this call back
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_data_to_vector);

  // argument to send to the write callback
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);

  // do it!
  code = curl_easy_perform(m_curl);

  // check the error code
  if (code != CURLE_OK) {
    return {false, curl_easy_strerror(code)};
  }

  return {true, "success"};
}