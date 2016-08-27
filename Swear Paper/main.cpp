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
const int WIDTH = 720;			// The resized image width in pix
const int HEIGHT = 480;			// the resized image height in pix
const int LINE_THICKNESS = 2;	// thickness of the lines used to write the text

//--------- FUNCTION PROTOTYPES --------
bool getImage(downloader &d, reddit_interface &e, cv::Mat &dst, std::string& fromJson);
int getRandomNumber(int max);
int getScaleFactor(int image_height);
int getWordWidth(const std::string& word);
int getWordHeight(const std::string& word);
cv::Scalar getTextColour(const cv::Mat& downloaded_image, const std::string& word);

//---------- MAIN -----------
int main(int argc, const char * argv[]) {
	
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
	
	// ---------- variables ---------
	downloader d;								// a downloader
	cv::Mat downloaded_image,					// place holder for the downloaded image
			text_image;							// The image with text on
	reddit_interface *e = new earthporn();		// this object knows about the structure of the json reply and where to get it form
	std::string result;							// the json returned as a string
	std::vector<std::string>	swearwords,		// the list which is in use throughout
								full_list;		// the vector of swear words from google
	const std::string window_name = "Output";	// the final window name
	
	// ------- init --------
	cv::namedWindow(window_name);				// make a window for the final display
	
	std::cout << "Getting swearwords" << std::endl;
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
			
			// if there was an error getting the json replay from reddit then stop doing img
			// processing and tidy stuff up
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
			
			// in order to write to the same image but wiht a different word I need to preserve
			// the original image, so make a copy in the text_image matrix
			downloaded_image.copyTo(text_image);
			
			// the images height and width
			int actual_h = downloaded_image.rows;
			int actual_w = downloaded_image.cols;
			
			// the location on the screen the text is going to go
			int x = actual_w/4;			// a quarter across
			int y = (actual_h/4) * 3;	// three quarters down
			
			// put the swearword on the image
			// near the bottom corner, in a white font with a fancy script looking font
			// autosizing it to scale with the image
			cv::putText(text_image, word, cv::Point(x, y), CV_FONT_HERSHEY_SCRIPT_COMPLEX, getScaleFactor(actual_h), getTextColour(downloaded_image, word), LINE_THICKNESS);
			
			std::cout << "Showing image" << std::endl;
			
			// show in a window and save the file so i can test both output formats
			// resize first so that it's manageable, as most of the images are high quality
			// although it's resized the text is put on the image first, so it still proves the
			// scaling of the text to the original image height
			// this make it difficult to read is the original image is really big, however its only for debugging
			// so dw about it for now
			// cv::resize(text_image, text_image, cv::Size(WIDTH, HEIGHT));
			
			cv::imshow(window_name, text_image);
			cv::imwrite("/Users/Shared/Swear Paper/test.jpg", text_image);
			
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

//----------------------
// Looks at the area where the text is going to go and returns the
// colour code for black of white, depending on whether the area is lighter or darker
cv::Scalar getTextColour(const cv::Mat& downloaded_image, const std::string& word){
	
	// the images' height and width
	int actual_h = downloaded_image.rows;
	int actual_w = downloaded_image.cols;
	
	// the location on the screen the text is going to go
	int x = actual_w/4;			// a quarter across
	int y = (actual_h/4) * 3;	// three quarters down
	
	int word_w = getWordWidth(word);
	int word_h = getWordHeight(word);
	
	// Create a rectanlge big enough for the word and at the same x and y co-ords as it
	cv::Rect r(x, y, word_w, word_h);
	
	// Create a new matrix using only the part of the downloaded image covered by the rectanlge
	cv::Mat roi(downloaded_image(r));
	cv::Mat grey;
	
	// convert to grey and threshold, this will turn the lightest pixels white and the darker ones black
	cv::cvtColor(roi, grey, CV_BGR2GRAY);
	
	//anything brighter than half will clip to 1, and anything less than hald will fall to 0
	threshold(grey, grey, 128, 255, cv::THRESH_BINARY);
	
	// Get the number of pixels in the rectanlge, then a count of this fully off and those fully on
	int numwhite = cv::countNonZero(grey);
	int total = word_w * word_h;
	int numBlack = total - numwhite;
	
	// if there are more white pixels, then I want the text on the image to be black, to
	// stand out. similarly if more white i want the text to be black
	bool text_is_black = numwhite > numBlack;
	
	// return the rgb values for white or black
	return text_is_black ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
}

//----------------------
// Gets the word's width in pixels
int getWordWidth(const std::string& word){
	return cv::getTextSize(word, CV_FONT_HERSHEY_SCRIPT_COMPLEX, 1, LINE_THICKNESS, nullptr).width;
}

//----------------------
// Gets the word height in pixels
int getWordHeight(const std::string& word){
	return cv::getTextSize(word, CV_FONT_HERSHEY_SCRIPT_COMPLEX, 1, LINE_THICKNESS, nullptr).height;
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
			// the url is the same as the last used one, so don't bother redownloading.
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
// Get the scale factor needed to make the text
// a 20th of the total height of the image
// using
//		desired height = image height / 20.0
//		scalefactor = desired height/ actual height
// doesn't take into account the length of the word
int getScaleFactor(int image_height){
	
	// default line thickness is 1, as is the scale factor, dont need a base point so nullptr
	// F is a tall letter so if F will fit then the rest will
	int text_height = getWordHeight("f");
	return double(image_height/20.0)/double(text_height);
}

//----------------------
// Gets a random number in the range 0 - max
int getRandomNumber(int max){
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	std::uniform_int_distribution<int> uni(0,max); // guaranteed unbiased
	return uni(rng);
}
