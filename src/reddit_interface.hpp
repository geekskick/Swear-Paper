//
//  reddit_interface.hpp
//  Swear Paper
//

#ifndef reddit_interface_hpp
#define reddit_interface_hpp

#include "json_parse_delegate_b.hpp"

#include <iostream>
#include <memory>
#include <string_view>

//--------- ABSTRACT CLASS -------
class reddit_interface {
public:
    reddit_interface(){};
    reddit_interface(std::shared_ptr<json_parse_delegate_b> d) : m_del(d){};
    virtual ~reddit_interface(){};
    virtual std::string get_url_from_reply(const std::string &json_to_parse, const int idx) const = 0;
    virtual std::string_view get_sub_reddit_url() const = 0;

protected:
    std::shared_ptr<json_parse_delegate_b> m_del;
};

#endif /* reddit_interface_hpp */
