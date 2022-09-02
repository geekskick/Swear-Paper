#ifndef IMAGE_DEL_HPP
#define IMAGE_DEL_HPP

#include <memory>
#include <string>

#include "image_location.hpp"
#include "image_size.hpp"
#include "program_delegate_b.hpp"

class image_delegate_b {
 public:
  image_delegate_b(std::shared_ptr<program_delegate_b> del)
      : m_del{std::move(del)} {}
  virtual ~image_delegate_b() = default;

  virtual void image_info(const std::string &info,
                          const image_size<int> &sz) const = 0;
  virtual void image_put_text(const std::string &text,
                              const image_location &loc) const = 0;
  virtual void image_saved(const std::string &fname) const = 0;

 protected:
  std::shared_ptr<program_delegate_b> m_del;
};

#endif
