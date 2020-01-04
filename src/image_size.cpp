#include "image_size.hpp"
#include <sstream>

std::string image_size::to_string() const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}
