//
//  downloader.hpp
//  Swear Paper
//

#ifndef downloader_hpp
#define downloader_hpp

#include <iostream>
#include <vector>

class downloader{
public:
	downloader();
	~downloader();
	
	std::pair<bool, std::string> perform_string(const std::string& url, std::string &result);
	std::pair<bool, std::string> perform_vector(const std::string& url, std::vector<std::string>& result);
	std::pair<bool, std::string> perform_image(const std::string& url, std::vector<char>& result);
	
private:
	void *m_curl;	// curl object
	
	// when data is recieved this function puts it into the stream
	static size_t write_data_to_string(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t write_data_to_vector(void *ptr, size_t size, size_t nmemb, void *stream);
};

#endif /* downloader_hpp */
