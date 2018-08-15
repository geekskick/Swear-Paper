//
//  earthporn.hpp
//  Swear Paper
//
//

#ifndef earthporn_hpp
#define earthporn_hpp

#include <iostream>
#include <json.hpp>
#include "reddit_interface.hpp"


//--------- EARTH PORN interface ----------
class earthporn: public nlohmann::json, public reddit_interface{
public:
	std::string get_url_from_reply(const std::string& json_to_parse, bool& is_new);
	std::string get_sub_reddit_url() const;
	
private:
	std::string m_previous_url;
	// thi url gets the posts from earthporn in json formatted with the newest at the top
	const std::string m_url = "https://www.reddit.com/r/earthporn/new.json?sort=new";
};


#endif /* earthporn_hpp */
