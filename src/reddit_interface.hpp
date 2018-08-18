//
//  reddit_interface.hpp
//  Swear Paper
//

#ifndef reddit_interface_hpp
#define reddit_interface_hpp

#include "json_parse_delegate_b.hpp"

#include <iostream>
#include <memory>

//--------- ABSTRACT CLASS -------
class reddit_interface {
public:
  reddit_interface(){};
  reddit_interface(std::shared_ptr<json_parse_delegate_b> d) : m_del(d){};
  virtual ~reddit_interface(){};
  virtual std::string get_url_from_reply(const std::string &json_to_parse,
                                         bool &is_new, const int idx) = 0;
  virtual std::string get_sub_reddit_url() const = 0;

protected:
  std::shared_ptr<json_parse_delegate_b> m_del;
};

#endif /* reddit_interface_hpp */
