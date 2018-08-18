//
//  main.cpp
//  Swear Paper
//

#include <boost/program_options.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <random>
#include <string>

#include "downloader.hpp"
#include "downloader_delegate.hpp"
#include "earthporn.hpp"
#include "image.hpp"
#include "json_parse_delegate.hpp"
#include "reddit_interface.hpp"

namespace po = boost::program_options;

static const std::string red_colour{"\u001b[31m"};
static const std::string reset_colour{"\u001b[0m"};
static const std::string green_colour{"\u001b[32m"};

//--------- FREE FUNCTION PROTOTYPES --------
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst,
               std::string &from_json, const int idx);
int get_random_number(int max);

//---------- MAIN -----------
int main(int argc, const char *argv[]) {
  std::shared_ptr<downloader_delegate_b> download_del{
      std::make_unique<downloader_delegate>()};
  std::shared_ptr<json_parse_delegate_b> parse_del{
      std::make_unique<json_parse_delegate>()};

  std::string swear_url{"https://raw.githubusercontent.com/LDNOOBW/"
                        "List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/"
                        "master/en"};
  std::string filename{"default"};

  std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

  po::options_description desc("Allowed Options");
  desc.add_options()("help", "Display help message")(
      "swears", po::value<std::string>(),
      "Specify the location of the swear word list")(
      "output", po::value<std::string>(), "Output filename");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(EXIT_SUCCESS);
  }

  if (vm.count("swears")) {
    swear_url = vm["swears"].as<std::string>();
  }

  if (vm.count("output")) {
    filename = vm["output"].as<std::string>();
    std::cout << "----\tOutputting to " << filename << std::endl;
  }

  std::cout << "----\tUsing " << swear_url << " as the source" << std::endl;

  downloader d(download_del);          // a downloader
  std::string json_str;                // the json returned as a string
  std::vector<std::string> swearwords; // the list which is in use throughout
  bool swears_done{true}; // the swear words are populated when this is true
  bool all_done{false};   // the image is made
  std::vector<char> raw_image;
  std::shared_ptr<reddit_interface> e{std::make_unique<earthporn>(parse_del)};

  std::cout << "----\tGetting swearwords" << std::endl;
  auto list = d.perform_vector(swear_url, swearwords);

  if (!list.first) {
    std::cerr << red_colour << "----\t" << list.second << reset_colour
              << std::endl;
    exit(EXIT_SUCCESS);
  }

  // If the swear words list isn't populated set the appropriate flag
  if (swearwords.empty()) {
    std::cerr << red_colour << "----\tSwear words not populated" << reset_colour
              << std::endl;
    exit(EXIT_FAILURE);
  }

  // If the swear words were downloaded then proceed to download the url
  // from the subreddit, else no point in wasting user time so skip to tidyup
  // section
  std::cout << "----\tGetting Json" << std::endl;

  //------------------------------------------------
  try {
    // get the json as a string
    auto rc = d.perform_string(e->get_sub_reddit_url(), json_str);
    if (!rc.first) {
      std::cerr << red_colour << "----\t" << rc.second << reset_colour
                << std::endl;
      exit(EXIT_FAILURE);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << __LINE__ << std::endl;

    // if there was an error getting the json reply from reddit then stop doing
    // img processing
    exit(EXIT_FAILURE);
  }

  std::cout << "----\tGetting Image" << std::endl;

  // If the image download was successful then do stuff,
  image downloaded_image;
  int idx = 0;
  bool retry = false;
  std::string word{""};
  do {
    if (get_image(d, *e, raw_image, json_str, idx)) {
      downloaded_image = image(raw_image);
      std::cout << "----\tThe image size is " << downloaded_image.size().w
                << "x" << downloaded_image.size().h << "." << std::endl;
      auto swear_copy{swearwords};

      do {
        // remember to prevent off by one errors
        int n = get_random_number((int)swear_copy.size() - 1);
        word = swear_copy[n];

        // remove it from the list so that it doesnt get selected again if it's
        // too big
        swear_copy.erase(swear_copy.begin() + n);
        std::cout << "----\tWord is:\t" << word << std::endl;

      } while (!downloaded_image.word_fits(word) && swear_copy.size() > 0);

      if (swear_copy.size() == 0) {
        std::cout << "----\tNo more swear words left to try and fit on, trying "
                     "a different image."
                  << std::endl;
        raw_image.clear();
        retry = true;
      }
    }
  } while (retry);

  downloaded_image.put_text(word);

  // construct the filename including extension
  if (0 >= vm.count("output")) {
    filename = word;
  }
  filename += ".jpg";

  std::cout << "----\tSaving image to " << filename << std::endl;

  downloaded_image.save_to_file(filename);
  std::cout << green_colour << "----\tDone, you " << word << reset_colour
            << std::endl;
}

//--------------------
// Gets the image described in the json reply from reddit, and puts it in the
// dst matrix returns true if theres wasn't an exception. The get_url_from_reply
// has stuff in it which might go wrong, so the try catch is needed to make sure
// parse errors are caught correctly
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst,
               std::string &from_json, const int idx) {
  if (!dst.empty()) {
    return true;
  }
  bool success{false};
  bool is_new{false};

  try {
    // the json reply has a url value in there, so let the reddit interface get
    // it out
    std::string url = e.get_url_from_reply(from_json, is_new, idx);

    // if the url is a new one then download a new image
    if (is_new) {
      std::cout << "----\tDownloading new image" << std::endl;
      auto rc{d.perform_image(url, dst)};
      if (!rc.first) {
        std::cerr << red_colour << "----\t" << rc.second << reset_colour
                  << std::endl;
        return false;
      }
    } else {
      // the url is the same as the last used one, so don't bother
      // redownloading.
      std::cout << "----\tNo new image, trying the next in the reply."
                << std::endl;
      return get_image(d, e, dst, from_json, idx + 1);
    }
    success = true;
  } catch (std::exception &e) {
    std::cout << e.what() << "\tline: " << __LINE__ << std::endl;
    throw;
  }

  return success;
}

//----------------------
// Gets a random number in the range 0 - max
int get_random_number(int max) {
  std::random_device rd; // only used once to initialise (seed) engine
  std::mt19937 rng(
      rd()); // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(0, max); // guaranteed unbiased
  return uni(rng);
}
