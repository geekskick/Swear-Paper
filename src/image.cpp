//
//  image.cpp
//  Swear Paper
//

#include "image.hpp"
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

std::string image_location::to_string(void) const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

std::string image_size::to_string(void) const {
    std::stringstream ss;
    ss << *this;
    return ss.str();
}

image::image(const int thick)
    :   m_line_thickness(thick), 
        m_font(CV_FONT_HERSHEY_SCRIPT_COMPLEX) 
{ }

image::image(image &rhs) {
    this->m_image = rhs.m_image;
    this->m_line_thickness = rhs.m_line_thickness;
    this->m_font = rhs.m_font;
}

image::image(std::vector<char> &from, const int thick) 
 : m_line_thickness(thick), 
 m_font(CV_FONT_HERSHEY_SCRIPT_COMPLEX), 
 m_image(cv::imdecode(from, -1)) 
 { }

void image::put_text(const std::string &word) {
    auto word_loc = word_location(word);
    cv::putText(m_image, word, cv::Point(word_loc.x, word_loc.y), m_font, scale_factor(), text_colour(word), m_line_thickness);
}

void image::save_to_file(const std::string &filename) const { cv::imwrite(filename, m_image); }

bool image::word_fits(const std::string &word) const {
    image_size total_sz{word_location(word).x + text_size(word).w, word_location(word).y + text_size(word).h};
    return total_sz.w < m_image.cols && total_sz.h < m_image.rows;
}

image_size image::size(void) { return {m_image.cols, m_image.rows}; }

cv::Scalar image::text_colour(const std::string &word) const {
    // the location on the screen the text is going to go
    image_location word_loc{word_location(word)};
    image_size word_size{text_size(word)};

    // Create a rectanlge big enough for the word and at the same x and y co-ords
    // as it
    cv::Rect r(word_loc.x, word_loc.y, word_size.w, word_size.h);

    // Create a new matrix using only the part of the downloaded image covered by
    // the rectanlge
    cv::Mat roi(m_image(r));
    cv::Mat grey;

    // convert to grey and threshold, this will turn the lightest pixels white and
    // the darker ones black
    cv::cvtColor(roi, grey, CV_BGR2GRAY);

    // anything brighter than half will remain the same, and anything less than
    // 128 will fall to 0 this part is the bit which is abit dodgy.
    threshold(grey, grey, 128, 255, cv::THRESH_TOZERO);

    // Get the number of pixels in the rectangle, then a count of this fully off
    // and those fully on
    int numwhite = cv::countNonZero(grey);
    int total = word_size.w * word_size.h;
    int numBlack = total - numwhite;

    // if there are more white pixels, then I want the text on the image to be
    // black, to stand out. similarly if more white i want the text to be black
    bool text_is_black = numwhite > numBlack;

    // return the rgb values for white or black
    return text_is_black ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
}

int image::scale_factor(void) const {
    int text_height{cv::getTextSize("f", m_font, 1, m_line_thickness, nullptr).height};
    int text_width{cv::getTextSize("f", m_font, 1, m_line_thickness, nullptr).width};

    return double(std::min(m_image.rows, m_image.cols) / 20.0) / double(std::max(text_height, text_width));
}

image_location image::word_location(const std::string &word) const {
    const auto sz{text_size(word)};
    return {(m_image.cols / 2) - (sz.w / 2), (m_image.rows / 2) - (sz.h / 2)};
}

image_size image::text_size(const std::string &word) const {
    auto sz = cv::getTextSize(word, m_font, scale_factor(), m_line_thickness, nullptr);
    return {sz.width, sz.height};
}
void image::init(void) {
    m_line_thickness = 2;
    m_font = CV_FONT_HERSHEY_SCRIPT_COMPLEX;
}
