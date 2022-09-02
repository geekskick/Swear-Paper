#include "include/downloader_delegate.hpp"

#include "interfaces/program_delegate_b.hpp"  // for program_delegate_b

void downloader_delegate::download_started(const std::string &url) { m_del->info("Started download from " + url); }
void downloader_delegate::download_ended(const std::string &url) { m_del->info("Finished download from " + url); }
