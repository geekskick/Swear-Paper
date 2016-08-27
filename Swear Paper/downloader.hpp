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
	
	std::string performString(const std::string& url);
	std::vector<std::string> performVector(const std::string& url);
	std::vector<char> performImage(const std::string& url);
	
private:
	void *m_curl;	// curl object
	
	// when data is recieved this function puts it into the stream
	static size_t write_dataToString(void *ptr, size_t size, size_t nmemb, void *stream);
	static size_t write_dataToVector(void *ptr, size_t size, size_t nmemb, void *stream);
};

#endif /* downloader_hpp */
