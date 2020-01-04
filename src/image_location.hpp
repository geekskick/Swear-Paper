#ifndef IMAGE_LOC_H
#define IMAGE_LOC_H

#include <ostream>

struct image_location {
    int x{};
    int y{};
    friend std::ostream &operator<<(std::ostream &os, const image_location &loc) {
        os << "(x: " << loc.x << ", y: " << loc.y << ")";
        return os;
    }
    std::string to_string() const;
};

#endif
