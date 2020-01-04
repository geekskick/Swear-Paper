#include "image_delegate.hpp"

void image_delegate::image_info(const std::string &msg, const image_size<int> &sz) const { m_del->info("Image info: " + msg + "\tImage size: " + sz.to_string()); }
void image_delegate::image_put_text(const std::string &text, const image_location &loc) const {
    m_del->info("Text put on image: " + text + ", at " + loc.to_string());
}
void image_delegate::image_saved(const std::string &text) const { m_del->success("Image saved as " + text); }
