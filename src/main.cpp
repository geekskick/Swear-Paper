//
//  main.cpp
//  Swear Paper
//

#include <iostream>
#include <random>
#include <memory>
#include <string>

#include "reddit_interface.hpp"
#include "earthporn.hpp"
#include "downloader.hpp"
#include "image.hpp"

//---------- CONSTANTS ----------
// there is a list of swear words at this address
const std::string SWEAR_URL = "https://raw.githubusercontent.com/LDNOOBW/List-of-Dirty-Naughty-Obscene-and-Otherwise-Bad-Words/master/en";

enum class args {
	EXEC_NAME = 0,
	OUTPUT_NAME = 1,
	COUNT = 2
};

//--------- FREE FUNCTION PROTOTYPES --------
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst, std::string& fromJson);
int get_random_number(int max);
void print_usage(void);

//---------- MAIN -----------
int main(int argc, const char * argv[]) {
	
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
	
	if(argc != static_cast<int>(args::COUNT)){
		print_usage();
		exit(EXIT_FAILURE);
	}
	
	downloader 					d;						// a downloader
	std::string 				result;					// the json returned as a string
	std::string					word;					// the swear word
	std::vector<std::string>	swearwords;				// the list which is in use throughout
	bool 						swears_done { true };	// the swear words are populated when this is true
	bool						all_done  	{ false };	// the image is made
	std::vector<char>			raw_image;
	std::shared_ptr<reddit_interface> e(new earthporn());
	
	std::cout << "Getting swearwords" << std::endl;
	swearwords = d.performVector(SWEAR_URL);
	
	// If the swear words list isn't populated set the appropriate flag
	if(swearwords.empty()){
		std::cout << "Swear words not populated" << std::endl;
		swears_done = false;
	}
	
	// If the swear words were downloaded then proceed to download the url
	// from the subreddit, else no point in wasting user time so skip to tidyup section
	if(swears_done){
		std::cout << "Getting Json" << std::endl;
		
		try{
			// get the json as a string
			result = d.performString(e->get_sub_reddit_url());
		}
		catch(std::exception& e){
			std::cout << e.what() << __LINE__ << std::endl;
			
			// if there was an error getting the json reply from reddit then stop doing img
			// processing and tidy stuff up
			exit(EXIT_FAILURE);
		}
	}
	else{
		exit(EXIT_FAILURE);
	}
	
	std::cout << "Getting Image" << std::endl;
	
	// If the image download was successful then do stuff,
	image downloaded_image;
	if(get_image(d, *e, raw_image, result)){
		
		downloaded_image = image(raw_image);
		do {
			// remember to prevent off by one errors
			int n = get_random_number((int)swearwords.size() - 1);
			word = swearwords[n];
			
			// remove it from the list so that it doesnt get selected again if it's too big
			swearwords.erase(swearwords.begin() + n);
			std::cout <<  "Word is:\t" << word << std::endl;
			
		} while (!downloaded_image.word_fits(word) && swearwords.size() > 0);
		
		if(swearwords.size() == 0){
			std::cout << "No more swear words left to try and fit on" << std::endl;
		}
		else{
			downloaded_image.put_text(word);
			
			// construct the filename including extension
			std::string filename(argv[static_cast<int>(args::OUTPUT_NAME)]);
			filename += ".jpg";
			
			std::cout << "Saving image to " << filename << std::endl;
			
			downloaded_image.save_to_file(filename);
			all_done = true;
		}
		
	}

	if(all_done){
		std::cout << "Done, you " << word << std::endl;
	}
	else{
		std::cout << "Not done" << std::endl;
	}
	
}

//----------------------
// Print the program usage
void print_usage(){
	std::cout << "Usage: ./swear_paper <output file name>" << std::endl;
}

//--------------------
// Gets the image described in the json reply from reddit, and puts it in the dst matrix
// returns true if theres wasn't an exception. The get_url_from_reply has stuff in it which might go wrong,
// so the try catch is needed to make sure parse errors are caught correctly
bool get_image(downloader &d, reddit_interface &e, std::vector<char> &dst, std::string& fromJson)
{
	bool success = false;
	bool is_new = false;
	
	try{
		// the json reply has a url value in there, so let the reddit interface get it out
		std::string url = e.get_url_from_reply(fromJson, is_new);
		
		// if the url is a new one then download a new image
		if(is_new){
			std::cout << "Downloading new image" << std::endl;
			dst = d.performImage(url);
		}
		else{
			// the url is the same as the last used one, so don't bother redownloading.
			std::cout << "No new image" << std::endl;
		}
		success = true;
	}
	catch(std::exception& e){
		std::cout << e.what() << "\tline: " << __LINE__ << std::endl;
	}
	
	return success;
}

//----------------------
// Gets a random number in the range 0 - max
int get_random_number(int max){
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0,max); // guaranteed unbiased
	return uni(rng);
}
