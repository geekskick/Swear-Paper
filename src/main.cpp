//
//  main.cpp
//  Swear Paper
//

#include <iostream>
#include <memory>
#include <random>
#include <string>

#include <boost/program_options.hpp>

#include "downloader.hpp"
#include "earthporn.hpp"
#include "image.hpp"
#include "reddit_interface.hpp"

namespace po = boost::program_options;

//--------- FREE FUNCTION PROTOTYPES --------
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst,
               std::string &fromJson);
int get_random_number(int max);
void print_usage(void);

//---------- MAIN -----------
int main(int argc, const char *argv[]) {
  std::string swear_url{
      "https://raw.githubusercontent.com/LDNOOBW/"
      "List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/master/en"};
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
    std::cout << "Outputting to " << filename << std::endl;
  }

  std::cout << "Using " << swear_url << " as the source" << std::endl;

  downloader d;                         // a downloader
  std::string result;                   // the json returned as a string
  std::string word;                     // the swear word
  std::vector<std::string> swearwords;  // the list which is in use throughout
  bool swears_done{true};  // the swear words are populated when this is true
  bool all_done{false};    // the image is made
  std::vector<char> raw_image;
  std::shared_ptr<reddit_interface> e(new earthporn());

  std::cout << "Getting swearwords" << std::endl;
  auto list = d.perform_vector(swear_url, swearwords);

  if (!list.first) {
    std::cerr << list.second << std::endl;
    exit(EXIT_SUCCESS);
  }

  // If the swear words list isn't populated set the appropriate flag
  if (swearwords.empty()) {
    std::cerr << "Swear words not populated" << std::endl;
    exit(EXIT_FAILURE);
  }

  // If the swear words were downloaded then proceed to download the url
  // from the subreddit, else no point in wasting user time so skip to tidyup
  // section
  std::cout << "Getting Json" << std::endl;

  //------------------------------------------------
  try {
    // get the json as a string
    auto rc = d.perform_string(e->get_sub_reddit_url(), result);
    if (!rc.first) {
      std::cerr << rc.second << std::endl;
      exit(EXIT_FAILURE);
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << __LINE__ << std::endl;

    // if there was an error getting the json reply from reddit then stop doing
    // img processing
    exit(EXIT_FAILURE);
  }

  std::cout << "Getting Image" << std::endl;

  // If the image download was successful then do stuff,
  image downloaded_image;
  if (get_image(d, *e, raw_image, result)) {
    downloaded_image = image(raw_image);
    std::cout << "The image size is " << downloaded_image.size().w << "x"
              << downloaded_image.size().h << std::endl;

    do {
      // remember to prevent off by one errors
      int n = get_random_number((int)swearwords.size() - 1);
      word = swearwords[n];

      // remove it from the list so that it doesnt get selected again if it's
      // too big
      swearwords.erase(swearwords.begin() + n);
      std::cout << "Word is:\t" << word << std::endl;

    } while (!downloaded_image.word_fits(word) && swearwords.size() > 0);

    if (swearwords.size() == 0) {
      std::cout << "No more swear words left to try and fit on" << std::endl;
      exit(EXIT_SUCCESS);
    }

    downloaded_image.put_text(word);

    // construct the filename including extension
    if (0 >= vm.count("output")) {
      filename = word;
    }
    filename += ".jpg";

    std::cout << "Saving image to " << filename << std::endl;

    downloaded_image.save_to_file(filename);
    std::cout << "Done, you " << word << std::endl;

  } else {
    std::cout << "get_image failed " << std::endl;
  }
}

//--------------------
// Gets the image described in the json reply from reddit, and puts it in the
// dst matrix returns true if theres wasn't an exception. The get_url_from_reply
// has stuff in it which might go wrong, so the try catch is needed to make sure
// parse errors are caught correctly
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst,
               std::string &fromJson) {
  bool success{false};
  bool is_new{false};

  try {
    // the json reply has a url value in there, so let the reddit interface get
    // it out
    std::string url = e.get_url_from_reply(fromJson, is_new);

    // if the url is a new one then download a new image
    if (is_new) {
      std::cout << "Downloading new image" << std::endl;
      auto rc{d.perform_image(url, dst)};
      if (!rc.first) {
        std::cerr << rc.second << std::endl;
        return false;
      }
    } else {
      // the url is the same as the last used one, so don't bother
      // redownloading.
      std::cout << "No new image" << std::endl;
    }
    success = true;
  } catch (std::exception &e) {
    std::cout << e.what() << "\tline: " << __LINE__ << std::endl;
  }

  return success;
}

//----------------------
// Gets a random number in the range 0 - max
int get_random_number(int max) {
  std::random_device rd;  // only used once to initialise (seed) engine
  std::mt19937 rng(
      rd());  // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(0, max);  // guaranteed unbiased
  return uni(rng);
}
