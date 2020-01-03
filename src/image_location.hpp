#ifndef IMAGE_LOC_H
#define IMAGE_LOC_H

#include <ostream>

class image_location {
public:
    int x;
    int y;
    friend std::ostream &operator<<(std::ostream &os, const image_location &loc) {
        os << "(x: " << loc.x << ", y: " << loc.y << ")";
        return os;
    }
    std::string to_string(void) const;
};

#endif
