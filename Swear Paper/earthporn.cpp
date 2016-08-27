//
//  earthporn.cpp
//  Swear Paper
//

#include "earthporn.hpp"

// Get the url I need from the json returned by reddit,
// if the url is a new one then make set is_new to true, else false
std::string earthporn::get_url_from_reply(const std::string& json_to_parse, bool &is_new){
	
	//this might throw an exception
	nlohmann::json j = parse(json_to_parse);
	
	// the json has a fixed structure and this is where the newest image link is
	std::string new_url = j["data"]["children"][0]["data"]["preview"]["images"][0]["source"]["url"];
	
	if(m_previous_url == new_url){
		is_new = false;
	}
	else{
		is_new = true;
		m_previous_url = new_url;
	}
	return new_url;
}

// get the subreddit url
std::string earthporn::get_sub_reddit_url() const{
	return m_url;
}