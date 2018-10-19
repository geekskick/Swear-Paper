//
//  earthporn.cpp
//  Swear Paper
//

#include "earthporn.hpp"

// Get the url I need from the json returned by reddit, idx is the image index
// in the number returned if the url is a new one then make set is_new to true,
// else false
std::string earthporn::get_url_from_reply(const std::string &json_to_parse, bool &is_new, const int idx = 0) {
    // this might throw an exception
    nlohmann::json j{parse(json_to_parse)};

    // the json has a fixed structure and this is where the newest image link is
    std::string new_url = j["data"]["children"][idx]["data"]["url"];

    if (m_del) {
        m_del->parse_result(new_url);
    }
    if (m_previous_url == new_url) {
        is_new = false;
    } else {
        is_new = true;
        m_previous_url = new_url;
    }
    return new_url;
}

// get the subreddit url
std::string earthporn::get_sub_reddit_url() const { return m_url; }