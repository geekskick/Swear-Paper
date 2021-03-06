#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "image_delegate_b.hpp"
#include "opencv2/core/core.hpp"
#include <memory>

class image {
public:
    image(const std::vector<char> &from, std::unique_ptr<image_delegate_b> del, const int thick = 1);

    void put_text(const std::string &word);
    void save_to_file(const std::string &filename) const;
    bool word_fits(const std::string &word) const;
    image_size<int> size();

private:
    cv::Mat m_image;
    int m_line_thickness;
    int m_font;
    std::unique_ptr<image_delegate_b> m_del;

    cv::Scalar text_colour(const std::string &word) const;
    int scale_factor() const;
    image_location word_location(const std::string &word) const;
    image_size<int> text_size(const std::string &word) const;
};

#endif
