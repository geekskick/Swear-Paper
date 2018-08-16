//
//  image.hpp
//  Swear Paper
//

#ifndef image_hpp
#define image_hpp

#include <opencv2/core/core.hpp>

struct location {
  int x;
  int y;
};

struct size {
  int w;
  int h;
};

class image {
 public:
  image(void);
  image(image& rhs);
  image(std::vector<char>& from);

  void put_text(const std::string& word);
  void save_to_file(const std::string& filename);
  bool word_fits(const std::string& word);
  struct size size(void);

 private:
  cv::Scalar text_colour(const std::string& word);
  int scale_factor(void);
  struct location word_location(void);
  void init(void);
  struct size text_size(const std::string& word);
  cv::Mat m_image;
  int m_line_thickness;
  int m_font;
};

#endif /* image_hpp */
