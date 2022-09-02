#include <fmt/ostream.h>

#include <boost/program_options.hpp>
#include <cstddef>
#include <memory>
#include <random>
#include <string>

#include "downloader.hpp"
#include "downloader_delegate.hpp"
#include "earthporn.hpp"
#include "image.hpp"
#include "image_delegate.hpp"
#include "json_parse_delegate.hpp"
#include "program_delegates.hpp"

namespace po = boost::program_options;
namespace {

int get_random_number(const int max) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, max);
  return uni(rng);
}
}  // namespace

template <>
struct fmt::formatter<po::options_description> : ostream_formatter {};

int main(int argc, const char *argv[]) {
  auto program_del = std::shared_ptr<program_delegate_b>{new verbose_program_delegate{}};
  auto download_del = std::unique_ptr<downloader_delegate_b>{new downloader_delegate{program_del}};
  auto parse_del = std::unique_ptr<json_parse_delegate_b>{new json_parse_delegate{program_del}};
  auto img_del = std::unique_ptr<image_delegate_b>{new image_delegate{program_del}};

  auto desc = po::options_description("Allowed Options");

  // clang-format off
  desc.add_options()
    ("help,h", "Display help message")
    ("source,s", po::value<std::string>()->default_value(
                                                    "https://raw.githubusercontent.com/LDNOOBW/"
                                                    "List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/master/en"), "Specify the location of the swear word list")
    ("output,o", po::value<std::string>(), "Output filename")
    ("quiet,q", "Don't show info messages")
    ("skip", po::value<int>()->default_value(0), "Skip to the nth image in the list of available ones")
    ("thickness", po::value<int>()->default_value(1), "Thickness of the line used to print the word");
  // clang-format on

  auto vm = po::variables_map();
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    fmt::print("{}\n", desc);
    exit(EXIT_SUCCESS);
  }

  if (vm.count("quiet")) {
    program_del = std::make_shared<quiet_program_delegate>();
  }

  const auto swear_url = vm["source"].as<std::string>();
  const auto idx = vm["skip"].as<int>();
  const auto thickness = vm["thickness"].as<int>();

  const auto d = downloader{std::move(download_del)};
  const auto e = earthporn{std::move(parse_del)};

  program_del->info("Getting swearwords from " + swear_url);
  auto swearwords = d.perform_vector(swear_url);
  if (!swearwords) {
    program_del->error("Unable to get swearwords");
    exit(EXIT_FAILURE);
  }

  if (swearwords->empty()) {
    program_del->error("Swear words not populated");
    exit(EXIT_FAILURE);
  }

  program_del->info(std::string{"Getting image json from "} + e.get_sub_reddit_url().data());

  // get the json as a string
  const auto json_string = d.perform_string(e.get_sub_reddit_url().data());
  if (!json_string) {
    program_del->error("Unable to get JSON");
    exit(EXIT_FAILURE);
  }

  const auto url = e.get_url_from_reply(*json_string, idx);
  const auto raw_image = d.perform_image(url);

  if (!raw_image) {
    program_del->error("Error getting the image");
    exit(EXIT_FAILURE);
  }

  auto downloaded_image = image(*raw_image, std::move(img_del), thickness);
  const auto word = [&]() {
    auto candidate = std::string{};
    do {
      const auto n = get_random_number(static_cast<int>(swearwords->size() - 1));
      candidate = swearwords->at(n);
      // remove it from the list so that it doesnt get selected again if it's
      // too big
      swearwords->erase(swearwords->begin() + n);
      program_del->info("Word is:\t " + candidate);

    } while (!downloaded_image.word_fits(candidate) && !swearwords->empty());

    if (swearwords->empty()) {
      program_del->error("No more swear words left to try and fit on.");
      exit(EXIT_FAILURE);
    }
    return candidate;
  }();

  downloaded_image.put_text(word);

  // construct the filename including extension
  const auto filename = [&]() {
    if (!vm.count("output")) {
      return word + ".jpg";
    }
    return vm["output"].as<std::string>() + ".jpg";
  }();

  downloaded_image.save_to_file(filename);
}
