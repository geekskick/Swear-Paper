//
//  image.hpp
//  Swear Paper
//

#ifndef image_hpp
#define image_hpp

#include <memory>
#include <opencv2/core/core.hpp>
#include <ostream>
#include <sstream>

#include "image_delegate_b.hpp"

class image {
public:
    image(std::shared_ptr<image_delegate_b> del, const int thick = 1);
    explicit image(const int thick = 1);
    image(image &rhs);
    image &operator=(image &rhs);
    image &operator=(image &&rhs);
    image(const std::vector<char> &from, std::shared_ptr<image_delegate_b> del, const int thick = 1);

    void put_text(const std::string &word);
    void save_to_file(const std::string &filename) const;
    bool word_fits(const std::string &word) const;
    image_size size(void);

private:
    cv::Mat m_image;
    int m_line_thickness;
    int m_font;
    std::shared_ptr<image_delegate_b> m_del;

    cv::Scalar text_colour(const std::string &word) const;
    int scale_factor(void) const;
    image_location word_location(const std::string &word) const;
    image_size text_size(const std::string &word) const;
};

#endif /* image_hpp */
