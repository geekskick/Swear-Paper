#ifndef IMAGE_SIZE_H
#define IMAGE_SIZE_H

#include <ostream>

class image_size {
public:
    int w;
    int h;
    friend std::ostream &operator<<(std::ostream &os, const image_size &sz) {
        os << "(w:" << sz.w << ", h: " << sz.h << ")";
        return os;
    }
    std::string to_string(void) const;
};
#endif
