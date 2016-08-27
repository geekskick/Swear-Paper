//
//  main.cpp
//  Swear Paper
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <random>

#include "reddit_interface.hpp"
#include "earthporn.hpp"
#include "downloader.hpp"

//---------- CONSTANTS ----------
const std::string SWEAR_URL = "https://gist.githubusercontent.com/ryanlewis/a37739d710ccdb4b406d/raw/0fbd315eb2900bb736609ea894b9bde8217b991a/google_twunter_lol";
const int WIDTH = 720, HEIGHT = 480;

//--------- FUNCTION PROTOTYPES --------
bool getImage(downloader &d, reddit_interface &e, cv::Mat &dst, std::string& fromJson);
int getRandomNumber(int max);

//---------- MAIN -----------
int main(int argc, const char * argv[]) {
	
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
	
	// ---------- variables ---------
	downloader d;								// a downloader
	cv::Mat downloaded_image,					// place holder forthe downloaded image
			text_image;							// The resized image with text on
	reddit_interface *e = new earthporn();		// this object knows about the structure of the json reply and where to get it form
	std::string result;							// the json returned as a string
	std::vector<std::string>	swearwords,		// the list which is in use throughout
								full_list;		// the vector of swear words from google
	const std::string window_name = "Output";	// the final window name
	
	// ------- init --------
	cv::namedWindow(window_name);				// make a window for the final display
	
	std::cout << "Getting sweatwords" << std::endl;
	full_list = d.performVector(SWEAR_URL);		// get the swear vector
	
	
	// ------- main loop --------
	while(1){
		
		std::cout << "Getting Json" << std::endl;
		
		try{
			// get the json as a string
			result = d.performString(e->get_sub_reddit_url());
		}
		catch(std::exception& e){
			std::cout << e.what() << __LINE__ << std::endl;
			break;
		}
		
		std::cout << "Getting Image" << std::endl;
		
		// If the image download was successful then do stuff, otherwise exit
		if(getImage(d, *e, downloaded_image, result)){
			
			// all the swear words are used up so get the list again
			if(swearwords.empty()){
				swearwords = full_list;
			}
			
			// get a new swear word and get rid of it from the list
			// remember to prevent off by one errors
			int n = getRandomNumber((int)swearwords.size() - 1);
			std::string word = swearwords[n];
			swearwords.erase(swearwords.begin() + n);
			
			std::cout <<  "Word is:\t" << word << std::endl;
			
			// make the image a reasonalble size
			cv::resize(downloaded_image, text_image, cv::Size(WIDTH, HEIGHT));
			
			// put the swearword on the image
			// near the bottom corner, in a white font with a fancy script looking font
			cv::putText(text_image, word, cv::Point(WIDTH/4,(HEIGHT/4)*3), CV_FONT_HERSHEY_SCRIPT_COMPLEX, 1, cv::Scalar(255,255,255));
			
			std::cout << "Showing image" << std::endl;
			
			cv::imshow(window_name, text_image);
			
			// wait 30secs or till a key is pressed to re-do it all
			cv::waitKey(30000);
			
		}
		else{
			break;
		}
	}
	
	// ------- clean up ------
	cv::destroyWindow("Output");
	delete e;
	std::cout << "Exitting" << std::endl;
	
}
//--------------------
// Gets the image described in the json reply from reddit, and puts it in the dst matrix
// returns true if theres wasn't an exception. The get_url_from_reply has stuff in it which might go wrong,
// so the try catch is needed to make sure parse errors are caught correctly
bool getImage(downloader &d, reddit_interface &e, cv::Mat &dst, std::string& fromJson)
{
	bool success = false;
	bool is_new = false;
	
	try{
		// the json reply has a url value in there, so let the reddit interface get it out
		std::string url = e.get_url_from_reply(fromJson, is_new);
		
		// if the url is a new one then download a new image
		if(is_new){
			std::cout << "Downloading new image" << std::endl;
			dst = cv::imdecode(d.performImage(url), -1);
		}
		else{
			std::cout << "No new image" << std::endl;
		}
		success = true;
	}
	catch(std::exception& e){
		std::cout << e.what() << __LINE__ << std::endl;
	}
	
	return success;
}

//----------------------
// Gets a random number in the range 0 - max
int getRandomNumber(int max){
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0,max); // guaranteed unbiased
	return uni(rng);
}
