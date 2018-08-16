//
//  reddit_interface.hpp
//  Swear Paper
//

#ifndef reddit_interface_hpp
#define reddit_interface_hpp

#include <iostream>

//--------- ABSTRACT CLASS -------
class reddit_interface {
 public:
  reddit_interface(){};
  virtual ~reddit_interface(){};
  virtual std::string get_url_from_reply(const std::string& json_to_parse,
                                         bool& is_new) = 0;
  virtual std::string get_sub_reddit_url() const = 0;
};

#endif /* reddit_interface_hpp */
