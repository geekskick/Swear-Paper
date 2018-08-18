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

#include "ansi_codes.hpp"
#include "downloader.hpp"
#include "downloader_delegate.hpp"
#include "earthporn.hpp"
#include "image.hpp"
#include "json_parse_delegate.hpp"
#include "program_delegates.hpp"
#include "reddit_interface.hpp"

namespace po = boost::program_options;

//--------- FREE FUNCTION PROTOTYPES --------
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst, std::string &from_json, const int idx,
               std::shared_ptr<program_delegate_b> &del);
int get_random_number(int max);

//---------- MAIN -----------
int main(int argc, const char *argv[]) {
    std::shared_ptr<program_delegate_b> program_del{std::make_shared<verbose_program_delegate>()};

    std::shared_ptr<downloader_delegate_b> download_del{std::make_shared<downloader_delegate>(program_del)};
    std::shared_ptr<json_parse_delegate_b> parse_del{std::make_shared<json_parse_delegate>(program_del)};

    std::string swear_url{"https://raw.githubusercontent.com/LDNOOBW/List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/master/en"};
    std::string filename{"default"};

    program_del->info("OpenCV Version: " + std::string(CV_VERSION));

    po::options_description desc("Allowed Options");
    desc.add_options()("help", "Display help message")("source", po::value<std::string>(), "Specify the location of the swear word list")(
        "output", po::value<std::string>(), "Output filename")("quiet", "Don't show info messages");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        exit(EXIT_SUCCESS);
    }

    if (vm.count("quiet")) {
        program_del->info("Changing to quiet mode, this is the last info message");
        program_del = std::make_shared<quiet_program_delegate>();
    }

    if (vm.count("source")) {
        swear_url = vm["source"].as<std::string>();
    }

    if (vm.count("output")) {
        filename = vm["output"].as<std::string>();
        program_del->info(filename);
    }

    downloader d(download_del);           // a downloader
    std::string json_str;                 // the json returned as a string
    std::vector<std::string> swearwords;  // the list which is in use throughout
    bool swears_done{true};               // the swear words are populated when this is true
    bool all_done{false};                 // the image is made
    std::vector<char> raw_image;
    earthporn e{parse_del};

    program_del->info("Getting swearwords from " + swear_url);
    auto list = d.perform_vector(swear_url, swearwords);

    if (!list.first) {
        program_del->error(list.second);
        exit(EXIT_SUCCESS);
    }

    // If the swear words list isn't populated set the appropriate flag
    if (swearwords.empty()) {
        program_del->error("Swear words not populated");
        exit(EXIT_SUCCESS);
    }

    // If the swear words were downloaded then proceed to download the url
    // from the subreddit, else no point in wasting user time so skip to tidyup
    // section
    program_del->info("Getting image json from " + e.get_sub_reddit_url());

    //------------------------------------------------
    try {
        // get the json as a string
        auto rc = d.perform_string(e.get_sub_reddit_url(), json_str);
        if (!rc.first) {
            program_del->error(rc.second);
            exit(EXIT_FAILURE);
        }
    } catch (std::exception &e) {
        program_del->error(e.what() + std::to_string(__LINE__));
        exit(EXIT_FAILURE);
    }

    // If the image download was successful then do stuff,
    image downloaded_image;
    int idx = 0;
    bool retry = false;
    std::string word{""};
    do {
        program_del->info("Downloading an image");
        if (get_image(d, e, raw_image, json_str, idx, program_del)) {
            downloaded_image = image(raw_image);
            program_del->info("The image size is " + downloaded_image.size().to_string());
            auto swear_copy{swearwords};

            do {
                // remember to prevent off by one errors
                int n = get_random_number((int)swear_copy.size() - 1);
                word = swear_copy[n];

                // remove it from the list so that it doesnt get selected again if it's
                // too big
                swear_copy.erase(swear_copy.begin() + n);
                program_del->info("Word is:\t " + word);

            } while (!downloaded_image.word_fits(word) && swear_copy.size() > 0);

            if (swear_copy.size() == 0) {
                program_del->error("No more swear words left to try and fit on, trying a different image.");
                raw_image.clear();
                retry = true;
            }
        }
    } while (retry);

    downloaded_image.put_text(word);

    // construct the filename including extension
    if (!vm.count("output")) {
        filename = word;
    }

    filename += ".jpg";

    downloaded_image.save_to_file(filename);
    program_del->success("Written to " + filename + " with the word '" + word + "'");
}

//--------------------
// Gets the image described in the json reply from reddit, and puts it in the
// dst matrix returns true if theres wasn't an exception. The get_url_from_reply
// has stuff in it which might go wrong, so the try catch is needed to make sure
// parse errors are caught correctly
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst, std::string &from_json, const int idx,
               std::shared_ptr<program_delegate_b> &del) {
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
            del->info("Downloading new image");
            auto rc{d.perform_image(url, dst)};
            if (!rc.first) {
                del->error(rc.second);
                return false;
            }
        } else {
            // the url is the same as the last used one, so don't bother
            // redownloading.
            del->info("No new image, trying the next in the reply.");
            return get_image(d, e, dst, from_json, idx + 1, del);
        }
        success = true;
    } catch (std::exception &e) {
        del->error(e.what() + std::string("\tline: ") + std::to_string(__LINE__));
        throw;
    }

    return success;
}

//----------------------
// Gets a random number in the range 0 - max
int get_random_number(int max) {
    std::random_device rd;                           // only used once to initialise (seed) engine
    std::mt19937 rng(rd());                          // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0, max);  // guaranteed unbiased
    return uni(rng);
}
