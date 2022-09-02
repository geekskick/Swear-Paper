#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <memory>                    // for unique_ptr
#include <opencv2/core/mat.hpp>      // for Mat
#include <opencv2/core/mat.inl.hpp>  // for Mat::~Mat
#include <opencv2/core/types.hpp>    // for Scalar
#include <string>                    // for string
#include <vector>                    // for vector

#include "image_location.hpp"               // for image_location
#include "image_size.hpp"                   // for image_size
#include "interfaces/image_delegate_b.hpp"  // for image_delegate_b

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
