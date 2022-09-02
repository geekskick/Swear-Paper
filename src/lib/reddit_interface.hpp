#ifndef REDDIT_INTERFACE_HPP
#define REDDIT_INTERFACE_HPP

#include <memory>
#include <string_view>

#include "json_parse_delegate_b.hpp"

class reddit_interface {
 public:
  reddit_interface(std::unique_ptr<json_parse_delegate_b> d)
      : m_del{std::move(d)} {};
  virtual ~reddit_interface() = default;
  virtual std::string get_url_from_reply(const std::string &json_to_parse,
                                         const int idx) const = 0;
  virtual std::string_view get_sub_reddit_url() const = 0;

 protected:
  std::unique_ptr<json_parse_delegate_b> m_del{};
};

#endif
