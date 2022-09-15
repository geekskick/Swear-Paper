#ifndef SRC_LIB_INCLUDE_INTERFACES_REDDIT_INTERFACE_HPP_
#define SRC_LIB_INCLUDE_INTERFACES_REDDIT_INTERFACE_HPP_

#include <memory>
#include <string>
#include <string_view>

class reddit_interface {
 public:
  virtual ~reddit_interface() = default;
  virtual std::string get_url_from_reply(const std::string &json_to_parse, const int idx) const = 0;
  virtual std::string_view get_sub_reddit_url() const = 0;
};

#endif  // SRC_LIB_INCLUDE_INTERFACES_REDDIT_INTERFACE_HPP_
