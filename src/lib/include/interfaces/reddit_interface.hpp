#ifndef REDDIT_INTERFACE_HPP
#define REDDIT_INTERFACE_HPP

#include <memory>
#include <string_view>

class reddit_interface {
 public:
  virtual ~reddit_interface() = default;
  virtual std::string get_url_from_reply(const std::string &json_to_parse, const int idx) const = 0;
  virtual std::string_view get_sub_reddit_url() const = 0;
};

#endif
