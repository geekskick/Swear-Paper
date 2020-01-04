#include "image_location.hpp"
#include <sstream>

std::string image_location::to_string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}
