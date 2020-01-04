//
//  earthporn.hpp
//  Swear Paper
//
//

#ifndef earthporn_hpp
#define earthporn_hpp

#include "reddit_interface.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

//--------- EARTH PORN interface ----------
class earthporn : public nlohmann::json, public reddit_interface {
public:
    earthporn(void) {}
    explicit earthporn(std::shared_ptr<json_parse_delegate_b> d) : reddit_interface(d) {}
    std::string get_url_from_reply(const std::string &json_to_parse, const int idx) const override;
    std::string get_sub_reddit_url() const override;

private:
    // thi url gets the posts from earthporn in json formatted with the newest at
    // the top
    const std::string m_url = "https://www.reddit.com/r/earthporn/new.json?sort=new";
};

#endif /* earthporn_hpp */
