#ifndef IMAGE_SIZE_HPP
#define IMAGE_SIZE_HPP

#include <ostream>
#include <sstream>

template <typename SizeType> struct image_size {

    SizeType w;
    SizeType h;

    friend std::ostream &operator<<(std::ostream &os, const image_size &sz) {
        os << "(w:" << sz.w << ", h: " << sz.h << ")";
        return os;
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << *this;
        return ss.str();
    }
};

#endif
