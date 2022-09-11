#include <fmt/ostream.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <boost/program_options.hpp>
#include <cstddef>
#include <fstream>
#include <iterator>
#include <memory>
#include <random>
#include <string>

#include "downloader.hpp"
#include "earthporn.hpp"
#include "environment_configuration.hpp"
#include "fmt_optional.hpp"
#include "image.hpp"

namespace po = boost::program_options;
namespace {

int get_random_number(const int max) {
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(0, max);
  return uni(rng);
}

std::vector<std::string> read_file_as_vector(const std::filesystem::path& filename) {
  auto file = std::ifstream{filename};
  auto rc = std::vector<std::string>{};
  std::copy(std::istream_iterator<std::string>(file), std::istream_iterator<std::string>(), std::back_inserter(rc));
  return rc;
}
}  // namespace

template <>
struct fmt::formatter<po::options_description> : ostream_formatter {};

int main(int argc, const char* argv[]) {
  spdlog::cfg::load_env_levels();
  const auto defaults = default_configuration{};
  const auto config = environment_configuration{defaults};

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

  const auto swear_url = vm["source"].as<std::string>();
  const auto idx = vm["skip"].as<int>();
  const auto thickness = vm["thickness"].as<int>();

  const auto d = downloader{};
  const auto e = earthporn{};

  const auto cache = config.cache_location();
  // if (!cache) {
  //  spdlog::debug("No cache detected, so downloading everything fresh");
  //} else {
  const auto swearsfile = cache.value() / "swears.txt";
  const auto swearsfile_exists = std::filesystem::exists(swearsfile);
  const auto cache_exists = std::filesystem::exists(cache.value());
  spdlog::trace("Cache {} exists: {}", cache, cache_exists);
  spdlog::debug("Swearsfile {} exists: {}", swearsfile, swearsfile_exists);

  if (!cache_exists) {
    spdlog::trace("Cache doesn't exist, so creating one");
    const auto created = std::filesystem::create_directories(cache.value());
    if (!created) {
      spdlog::warn("Unable to create cache location: {}", cache);
    }
  }
  auto swearwords = [&]() {
    if (swearsfile_exists) {
      return read_file_as_vector(swearsfile);
    } else {
      spdlog::debug("Getting swearwords from " + swear_url);
      const auto downloaded_swearwords = d.perform_vector(swear_url);

      if (!downloaded_swearwords) {
        spdlog::error("Unable to get swearwords");
        exit(EXIT_FAILURE);
      }
      spdlog::trace("Creating a cached swears file");
      auto file = std::ofstream{swearsfile};
      for (const auto& word : downloaded_swearwords.value()) {
        file << word << "\n";
      }
      return downloaded_swearwords.value();
    }
  }();

  if (swearwords.empty()) {
    spdlog::error("Swear words not populated");
    exit(EXIT_FAILURE);
  }

  spdlog::debug(std::string{"Getting image json from "} + e.get_sub_reddit_url().data());

  // get the json as a string
  const auto json_string = d.perform_string(e.get_sub_reddit_url().data());
  if (!json_string) {
    spdlog::error("Unable to get JSON");
    exit(EXIT_FAILURE);
  }

  const auto url = e.get_url_from_reply(*json_string, idx);
  const auto raw_image = d.perform_image(url);

  if (!raw_image) {
    spdlog::error("Error getting the image");
    exit(EXIT_FAILURE);
  }

  auto downloaded_image = image(*raw_image, thickness);
  const auto word = [&]() {
    auto candidate = std::string{};
    do {
      const auto n = get_random_number(static_cast<int>(swearwords.size() - 1));
      candidate = swearwords.at(n);
      // remove it from the list so that it doesnt get selected again if it's
      // too big
      swearwords.erase(swearwords.begin() + n);
      spdlog::debug("Word is:\t " + candidate);

    } while (!downloaded_image.word_fits(candidate) && !swearwords.empty());

    if (swearwords.empty()) {
      spdlog::error("No more swear words left to try and fit on.");
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
