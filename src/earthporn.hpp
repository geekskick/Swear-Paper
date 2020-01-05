#ifndef EARTHPORN_HPP
#define EARTHPORN_HPP

#include "nlohmann/json.hpp"
#include "reddit_interface.hpp"
#include <string_view>

//--------- EARTH PORN interface ----------
class earthporn : public nlohmann::json, public reddit_interface {
public:
    explicit earthporn(std::unique_ptr<json_parse_delegate_b> d) : reddit_interface{std::move(d)} {}
    std::string get_url_from_reply(const std::string &json_to_parse, const int idx) const override;
    std::string_view get_sub_reddit_url() const override;

private:
    // thi url gets the posts from earthporn in json formatted with the newest at
    // the top
    constexpr static std::string_view m_url{"https://www.reddit.com/r/earthporn/new.json?sort=new"};
};

#endif /* earthporn_hpp */
