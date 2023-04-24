#ifndef SRC_LIB_INCLUDE_EARTHPORN_HPP_
#define SRC_LIB_INCLUDE_EARTHPORN_HPP_

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

#include "interfaces/reddit_interface.hpp"

//--------- EARTH PORN interface ----------
class earthporn : public nlohmann::json, public reddit_interface {
 public:
  std::string get_url_from_reply(const std::string_view json_to_parse, const int idx) const override;
  std::string_view get_sub_reddit_url() const override;

 private:
  // thi url gets the posts from earthporn in json formatted with the newest at
  // the top
  constexpr static std::string_view m_url{"https://www.reddit.com/r/earthporn/new.json?sort=new"};
};

#endif  // SRC_LIB_INCLUDE_EARTHPORN_HPP_
