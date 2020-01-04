//
//  earthporn.cpp
//  Swear Paper
//

#include "earthporn.hpp"

std::string earthporn::get_url_from_reply(const std::string &json_to_parse, const int idx = 0) const {
    // this might throw an exception
    const auto j = nlohmann::json{parse(json_to_parse)};

    // the json has a fixed structure and this is where the newest image link is
    const auto new_url = j["data"]["children"][idx]["data"]["url"];

    if (m_del) {
        m_del->parse_result(new_url);
    }

    return new_url;
}

// get the subreddit url
std::string earthporn::get_sub_reddit_url() const { return m_url; }
