//
//  image.hpp
//  Swear Paper
//

#ifndef image_hpp
#define image_hpp

#include <opencv2/core/core.hpp>
#include <ostream>

class image_location {
   public:
    int x;
    int y;
    friend std::ostream &operator<<(std::ostream &os, const image_location &loc) {
        os << "(x: " << loc.x << ", y: " << loc.y << ")";
        return os;
    }
};

class image_size {
   public:
    int w;
    int h;
    friend std::ostream &operator<<(std::ostream &os, const image_size &sz) {
        os << "(w:" << sz.w << ", h: " << sz.h << ")";
        return os;
    }
};

class image {
   public:
    image(void);
    image(image &rhs);
    image(std::vector<char> &from);

    void put_text(const std::string &word);
    void save_to_file(const std::string &filename);
    bool word_fits(const std::string &word);
    image_size size(void);

   private:
    cv::Scalar text_colour(const std::string &word);
    int scale_factor(void);
    image_location word_location(const std::string &word);
    void init(void);
    image_size text_size(const std::string &word);
    cv::Mat m_image;
    int m_line_thickness;
    int m_font;
};

#endif /* image_hpp */
