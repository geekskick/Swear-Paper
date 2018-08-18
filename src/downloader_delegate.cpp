#include "downloader_delegate.hpp"
#include <iostream>

void downloader_delegate::download_started(const std::string &url) {
  std::cout << "Started download from " << url << std::endl;
}
void downloader_delegate::download_ended(const std::string &url) {
  std::cout << "Finished download from " << url << std::endl;
}
