#ifndef SRC_LIB_INCLUDE_IMAGE_HPP_
#define SRC_LIB_INCLUDE_IMAGE_HPP_

#include <filesystem>
#include <memory>                    // for unique_ptr
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/types.hpp>    // for Scalar
#include <string>                    // for string
#include <vector>                    // for vector

#include "image_location.hpp"  // for image_location
#include "image_size.hpp"      // for image_size

class image {
 public:
  explicit image(const std::vector<char> &from, const int thick = 1);

  void put_text(const std::string_view &word);
  void save_to_file(const std::filesystem::path &filepath) const;
  bool word_fits(const std::string_view &word) const;
  image_size<int> size();

 private:
  cv::Mat m_image;
  int m_line_thickness;
  int m_font;

  cv::Scalar text_colour(const std::string_view &word) const;
  int scale_factor() const;
  image_location word_location(const std::string_view &word) const;
  image_size<int> text_size(const std::string_view &word) const;
};

#endif  // SRC_LIB_INCLUDE_IMAGE_HPP_
