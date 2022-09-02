#ifndef IMAGE_DELEGATE_HPP
#define IMAGE_DELEGATE_HPP

#include <memory>  // for shared_ptr
#include <string>  // for string

#include "interfaces/image_delegate_b.hpp"  // for image_delegate_b
class program_delegate_b;
struct image_location;
template <typename SizeType>
struct image_size;

class image_delegate : public image_delegate_b {
 public:
  explicit image_delegate(std::shared_ptr<program_delegate_b> &del) : image_delegate_b(del) {}

  void image_info(const std::string &info, const image_size<int> &sz) const override;
  void image_put_text(const std::string &text, const image_location &loc) const override;
  void image_saved(const std::string &text) const override;
};

#endif
