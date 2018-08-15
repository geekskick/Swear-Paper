//
//  downloader.cpp
//  Swear Paper
//

#include "downloader.hpp"
#include <sstream>
#include <curl/curl.h>
#include <curl/easy.h>

// constructor inits libcurl
downloader::downloader(){
	m_curl = curl_easy_init();
}

// dtor safely cleans up the downloader
downloader::~downloader(){
	curl_easy_cleanup(m_curl);
}

// Callback for putting the data rx'd into a string
size_t downloader::write_dataToString(void *ptr, size_t size, size_t nmemb, void *stream) {
	std::string data((const char*) ptr, (size_t) size * nmemb); // create new string from the data
	*((std::string*)stream) += data;	// add the data to the out string
	return size * nmemb;
}

// Do the download and get the reply as a string
std::string downloader::performString(const std::string& url){
	
	std::string result = "";	// dest string
	CURLcode code;				// error codes
	
	// connect to the url
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	
	// when reply is recieved from the write function it goes to this call back
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_dataToString);
	
	// argument to send to the write callback
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);
	
	// do it!
	code = curl_easy_perform(m_curl);
	
	// check the error code
	if(code != CURLE_OK){
		result = "";
		std::cout << curl_easy_strerror(code);
	}
	
	return result;
}

// Do the download and get the reply as a vector of strings
std::vector<std::string> downloader::performVector(const std::string& url){
	
	std::string reply_as_string = performString(url);	// the big string
	std::string to;										// used for iterationg
	std::stringstream stream(reply_as_string);			// The big reply as a stringstream
	std::vector<std::string> words;						// the final vector
	
	// iterate over the big string and put the lines into the 'to' variable, then push that to the back of the vector
	while(std::getline(stream, to, '\n')){
		words.push_back(to);
		
	}
	
	return words;
}

// Callback for putting the data rx'd into a vector of chars
size_t downloader::write_dataToVector(void *inptr, size_t size, size_t nmemb, void *userdata){
	char *ptr = (char*)inptr;
	std::vector<char> *stream = (std::vector<char>*)userdata;
	size_t count = size * nmemb;
	stream->insert(stream->end(), ptr, ptr + count);
	return count;
}

// Do the download and get the reply as a vector of charactrers (image)!
std::vector<char> downloader::performImage(const std::string& url){
	
	std::vector<char> result;
	CURLcode code;
	
	// connect to the url
	curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
	
	// when reply is recieved from the write function it goes to this call back
	curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_dataToVector);
	
	// argument to send to the write callback
	curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &result);
	
	// do it!
	code = curl_easy_perform(m_curl);
	
	// check the error code
	if(code != CURLE_OK){
		std::cout << curl_easy_strerror(code);
	}
	
	return result;
}