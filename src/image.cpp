//
//  image.cpp
//  Swear Paper
//

#include "image.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

image::image(const int thick) : m_line_thickness(thick), m_font(cv::FONT_HERSHEY_SCRIPT_COMPLEX) {}

image::image(image &rhs) : m_image(rhs.m_image), m_line_thickness(rhs.m_line_thickness), m_font(rhs.m_font), m_del(rhs.m_del) {}

image::image(std::shared_ptr<image_delegate_b> del, const int thick) : m_line_thickness(thick), m_font(cv::FONT_HERSHEY_SCRIPT_COMPLEX), m_del(del) {}

image::image(const std::vector<char> &from, std::shared_ptr<image_delegate_b> del, const int thick)
    : m_image(cv::imdecode(from, -1)), m_line_thickness(thick), m_font(cv::FONT_HERSHEY_SCRIPT_COMPLEX), m_del(del) {
    if (m_del) {
        m_del->image_info("Dimensions", size());
    }
}

void image::put_text(const std::string &word) {
    const auto word_loc = word_location(word);
    cv::putText(m_image, word, cv::Point(word_loc.x, word_loc.y), m_font, scale_factor(), text_colour(word), m_line_thickness);
    if (m_del) {
        m_del->image_put_text(word, word_loc);
    }
}

void image::save_to_file(const std::string &filename) const {
    cv::imwrite(filename, m_image);
    if (m_del) {
        m_del->image_saved(filename);
    }
}

bool image::word_fits(const std::string &word) const {
    const auto total_sz = image_size<int>{word_location(word).x + text_size(word).w, word_location(word).y + text_size(word).h};
    return total_sz.w < m_image.cols && total_sz.h < m_image.rows;
}

image_size<int> image::size() { return {m_image.cols, m_image.rows}; }

cv::Scalar image::text_colour(const std::string &word) const {
    // the location on the screen the text is going to go
    const auto word_loc = image_location{word_location(word)};
    const auto word_size = image_size<int>{text_size(word)};

    // Create a rectanlge big enough for the word and at the same x and y co-ords
    // as it
    cv::Rect r(word_loc.x, word_loc.y, word_size.w, word_size.h);

    // Create a new matrix using only the part of the downloaded image covered by
    // the rectanlge
    cv::Mat roi(m_image(r));
    cv::Mat grey;

    // convert to grey and threshold, this will turn the lightest pixels white and
    // the darker ones black
    cv::cvtColor(roi, grey, cv::COLOR_BGR2GRAY);

    // anything brighter than half will remain the same, and anything less than
    // 128 will fall to 0 this part is the bit which is abit dodgy.
    threshold(grey, grey, 128, 255, cv::THRESH_TOZERO);

    // Get the number of pixels in the rectangle, then a count of this fully off
    // and those fully on
    const int numwhite = cv::countNonZero(grey);
    const int total = word_size.w * word_size.h;
    const int numBlack = total - numwhite;

    // if there are more white pixels, then I want the text on the image to be
    // black, to stand out. similarly if more white i want the text to be black
    const bool text_is_black = numwhite > numBlack;

    // return the rgb values for white or black
    return text_is_black ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
}

int image::scale_factor() const {
    const int text_height{cv::getTextSize("f", m_font, 1, m_line_thickness, nullptr).height};
    const int text_width{cv::getTextSize("f", m_font, 1, m_line_thickness, nullptr).width};

    return double(std::min(m_image.rows, m_image.cols) / 20.0) / double(std::max(text_height, text_width));
}

image_location image::word_location(const std::string &word) const {
    const auto sz{text_size(word)};
    return {(m_image.cols / 2) - (sz.w / 2), (m_image.rows / 2) - (sz.h / 2)};
}

image_size<int> image::text_size(const std::string &word) const {
    auto sz = cv::getTextSize(word, m_font, scale_factor(), m_line_thickness, nullptr);
    return {sz.width, sz.height};
}

image &image::operator=(image &rhs) {
    this->m_image = rhs.m_image;
    this->m_line_thickness = rhs.m_line_thickness;
    this->m_font = rhs.m_font;
    this->m_del = rhs.m_del;
    return *this;
}
image &image::operator=(image &&rhs) {
    this->m_image = rhs.m_image;
    this->m_line_thickness = rhs.m_line_thickness;
    this->m_font = rhs.m_font;
    this->m_del = rhs.m_del;
    return *this;
}
