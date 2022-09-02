#include "include/image_delegate.hpp"

#include <fmt/core.h>  // for basic_string_view, format

#include "interfaces/program_delegate_b.hpp"  // for program_delegate_b
struct image_location;
template <typename SizeType>
struct image_size;

void image_delegate::image_info(const std::string &info, const image_size<int> &sz) const { m_del->info(fmt::format("Image {}:\tImage size: {}", info, sz)); }
void image_delegate::image_put_text(const std::string &text, const image_location &loc) const { m_del->info(fmt::format("Text put on image: {}, at {}", text, loc)); }
void image_delegate::image_saved(const std::string &text) const { m_del->success("Image saved as " + text); }
