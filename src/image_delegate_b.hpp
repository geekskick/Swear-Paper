#ifndef IMAGE_DEL_H
#define IMAGE_DEL_H

#include <memory>
#include <string>
#include "image_location.hpp"
#include "image_size.hpp"
#include "program_delegate_b.hpp"

class image_delegate_b {
   public:
    image_delegate_b(std::shared_ptr<program_delegate_b>& del) : m_del(del) {}
    virtual ~image_delegate_b(void) {}

    virtual void image_info(const std::string& info, const image_size& sz) const = 0;
    virtual void image_put_text(const std::string& text, const image_location& loc) const = 0;
    virtual void image_saved(const std::string& fname) const = 0;

   protected:
    std::shared_ptr<program_delegate_b>& m_del;
};

#endif