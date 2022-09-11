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

template <typename T>
struct CacheOrDownload {
  std::filesystem::path cache_location{};
  std::string_view download_url{};
  const downloader& d;
  std::optional<T> value{};
  bool value_set{false};

  void create_cache(const T& to_write) {
    spdlog::trace("Creating a cached value file: ", cache_location);
    auto file = std::ofstream{cache_location};
    for (const auto& word : to_write) {
      file << word << "\n";
    }
  }
  CacheOrDownload(const std::filesystem::path location, std::string_view url, const downloader& download)
      : cache_location{std::move(location)}, download_url{std::move(url)}, d{download} {}

  [[nodiscard]] const std::optional<T>& get() {
    if (value_set) {
      spdlog::trace("Value already set, so not attempting to get again");
      return value;
    }
    const auto cache_exists = std::filesystem::exists(cache_location);
    if (cache_exists) {
      spdlog::trace("Cache found at {}, so using the contents of that", cache_location);
      value = read_file_as_vector(cache_location);
      value_set = true;
      return value;
    }

    spdlog::debug("Getting value from {}", download_url);
    const auto downloaded_value = d.perform_vector(std::string{download_url});

    if (!downloaded_value) {
      spdlog::error("Unable to get value from {}", download_url);
      return value;
    }

    create_cache(downloaded_value.value());

    spdlog::trace("Providing value");
    value = std::move(downloaded_value);
    value_set = true;
    return value;
  }

  [[nodiscard]] operator bool() const { return value; }
};
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

  const auto curl_init = curl_global_init(CURL_GLOBAL_ALL);
  if (curl_init != CURLE_OK) {
    spdlog::error("Unable to initialise curl");
    exit(EXIT_FAILURE);
  }

  const auto cleanup_set_failure = std::atexit([]() {
    spdlog::trace("Cleaning up curl");
    curl_global_cleanup();
  });

  if (cleanup_set_failure) {
    spdlog::error("Unable to set curl cleanup");
  }

  const auto swear_url = vm["source"].as<std::string>();
  const auto idx = vm["skip"].as<int>();
  const auto thickness = vm["thickness"].as<int>();

  const auto swears_downloader = downloader{};
  const auto image_downloader = downloader{};
  const auto e = earthporn{};

  const auto cache = config.cache_location();
  const auto swearsfile = cache.value() / "swears.txt";
  const auto cache_exists = std::filesystem::exists(cache.value());

  if (!cache_exists) {
    spdlog::trace("Cache directory {} doesn't exist, so creating one", cache);
    const auto created = std::filesystem::create_directories(cache.value());
    if (!created) {
      spdlog::warn("Unable to create cache directory: {}", cache);
    }
  }

  auto swearword_provider = CacheOrDownload<std::vector<std::string>>{swearsfile, swear_url, swears_downloader};
  auto swearwords_maybe = std::optional<std::vector<std::string>>{};

  auto raw_image = std::optional<std::vector<char>>{};
  auto image_thread = std::thread([&]() {
    spdlog::debug(std::string{"Getting image json from "} + e.get_sub_reddit_url().data());

    // get the json as a string
    const auto json_string = image_downloader.perform_string(e.get_sub_reddit_url().data());
    if (!json_string) {
      spdlog::error("Unable to get JSON");
      exit(EXIT_FAILURE);
    }

    const auto url = e.get_url_from_reply(*json_string, idx);
    raw_image = image_downloader.perform_image(url);
  });

  auto swears_thread = std::thread([&]() { swearwords_maybe = swearword_provider.get(); });
  swears_thread.join();
  spdlog::trace("Swears thread done");

  if (!swearwords_maybe) {
    spdlog::error("Unable to get swear words!");
    exit(EXIT_FAILURE);
  }

  auto swearwords = swearwords_maybe.value();

  if (swearwords.empty()) {
    spdlog::error("Swear words not populated");
    exit(EXIT_FAILURE);
  }

  image_thread.join();
  spdlog::trace("Image thread done");
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
