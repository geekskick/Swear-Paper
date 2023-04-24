#include "include/earthporn.hpp"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
std::string earthporn::get_url_from_reply(const std::string_view json_to_parse, const int idx = 0) const {
  // this might throw an exception
  try {
    const auto json = nlohmann::json::parse(json_to_parse);

    // the json has a fixed structure and this is where the newest image link is
    const auto new_url = json["data"]["children"][idx]["data"]["url"];

    spdlog::debug("Parsed URL {}", new_url);

    return new_url;
  } catch (const nlohmann::json::exception& e) {
    throw std::runtime_error(fmt::format("Unable to parse the json response {}: {}", e.what(), json_to_parse));
  }
}

// get the subreddit url
std::string_view earthporn::get_sub_reddit_url() const { return m_url; }
