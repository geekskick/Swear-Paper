//
//  downloader.hpp
//  Swear Paper
//

#ifndef downloader_hpp
#define downloader_hpp

#include <curl/curl.h>
#include <string>
#include <vector>
#include "downloader_delegate_b.hpp"

class downloader {
   public:
    downloader(void) : m_del(nullptr) {}
    downloader(std::shared_ptr<downloader_delegate_b> del) : m_del(del) {}
    virtual ~downloader(void) {}
    virtual std::string get_str(const std::string& from) const = 0;
    virtual std::vector<std::string> get_vec(const std::string& from) const = 0;
    virtual std::vector<char> get_img(const std::string& from) const = 0;
    virtual void set_delegate(std::shared_ptr<downloader_delegate_b> del) { m_del = del; }

   protected:
    std::shared_ptr<downloader_delegate_b> m_del;
};

class curl_downloader : public downloader {
   public:
    curl_downloader(void) : m_curl(curl_easy_init()) {}
    curl_downloader(std::shared_ptr<downloader_delegate_b> del) : downloader(del), m_curl(curl_easy_init()) {}
    ~curl_downloader(void) { curl_easy_cleanup(m_curl); }

    std::vector<std::string> get_vec(const std::string& from) const;
    std::vector<char> get_img(const std::string& from) const;
    std::string get_str(const std::string& from) const;

   private:
    void* m_curl;
    static size_t write_data_to_string(void* ptr, size_t size, size_t nmemb, void* stream);
    void check_rc(const CURLcode& rc, const std::string& msg) const;
};

#endif /* downloader_hpp */
