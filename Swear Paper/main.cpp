//
//  main.cpp
//  Swear Paper
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <random>
#include <memory>

#include "reddit_interface.hpp"
#include "earthporn.hpp"
#include "downloader.hpp"

//---------- CONSTANTS ----------
// there is a list of swear words at this address
const std::string SWEAR_URL = "https://gist.githubusercontent.com/ryanlewis/a37739d710ccdb4b406d/raw/0fbd315eb2900bb736609ea894b9bde8217b991a/google_twunter_lol";
const int LINE_THICKNESS = 2;	// thickness of the lines used to write the text
enum ARGS { EXEC_NAME, OUTPUT_NAME, COUNT }; // the arguments expected

//--------- FUNCTION PROTOTYPES --------
// Yay for inconsistent style! I would prefer these to be get_image etc, but
// xcode doesnt allow for c++ refactoring like that and im lazy
bool getImage(downloader &d, reddit_interface &e, cv::Mat &dst, std::string& fromJson);
int getRandomNumber(int max);
int getScaleFactor(int image_height);
int getWordWidth(const std::string& word, const int sf);
int getWordHeight(const std::string& word, const int sf);
cv::Scalar getTextColour(const cv::Mat& downloaded_image, const std::string& word, const int sf);
void printUsage();
int getWordXPos(const int image_w);
int getWordYPos(const int image_h);
//---------- MAIN -----------
int main(int argc, const char * argv[]) {
	
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
	
	// if the usage isn't as expected then let th user know how to
	// use it, then exit the application.
	// no need to delete objects etc as its not constructed yet.
	if(argc != ARGS::COUNT){
		printUsage();
		exit(EXIT_FAILURE);
	}
	
	// ---------- variables ---------
	downloader d;								// a downloader
	cv::Mat downloaded_image;					// place holder for the downloaded image
	std::string result,							// the json returned as a string
				word;							// the swear word
	std::vector<std::string>	swearwords;		// the list which is in use throughout
	bool swears_done = true,					// the swear words are populated when this is true
		all_done = false;						// the image is made
	
	// the interface needs to be a pointer to allow the strategy to work, so
	// use a smart pointer so that i can pass it to functions, and not worry
	// about safely exitting the application
	std::shared_ptr<reddit_interface> e(new earthporn());
	
	// ------- init --------
	std::cout << "Getting swearwords" << std::endl;
	swearwords = d.performVector(SWEAR_URL);	// get the swears vector
	
	// If the swear words list isn't populated set the appropriate flag
	if(swearwords.empty()){
		std::cout << "Swear words not populated" << std::endl;
		swears_done = false;
	}
	
	// ------- main loop --------
	
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
	if(getImage(d, *e, downloaded_image, result)){
		
		// get the image size
		int actual_h = downloaded_image.rows;
		int actual_w = downloaded_image.cols;
		
		int text_x;	// the text topleft x coord
		int text_y;	// the text topleft y coord
		int sf;		// the test scalefactor to make it fit on the image
		
		int width_limit;	// the last part of the word across the image
		int height_limit;	// the last part of the word down the image
		
		do {
			// get a new swear word and get rid of it from the list
			// remember to prevent off by one errors
			int n = getRandomNumber((int)swearwords.size() - 1);
			word = swearwords[n];
			
			// remove it from the list so that it doesnt get selected again if it's too big
			swearwords.erase(swearwords.begin() + n);
			
			std::cout <<  "Word is:\t" << word << std::endl;
			
			// the location on the screen the text is going to go
			text_x = getWordXPos(actual_w);		// a quarter across
			text_y = getWordYPos(actual_h);		// three quarters down
			
			sf = getScaleFactor(actual_h);
			
			height_limit = getWordYPos(actual_h) + getWordHeight(word, sf);
			width_limit = (getWordXPos(actual_w) + getWordWidth(word, sf));
		} while ( height_limit >= actual_h ||	// make sure that the word fits on the height of the image
				 width_limit > actual_w);		// makesure that the word fits on the width of the image (the more likely one to fail)!
		
		cv::putText(downloaded_image,								// on the downloaded image
					word,											// the swear word
					cv::Point(text_x, text_y),						// it's topleft location
					CV_FONT_HERSHEY_SCRIPT_COMPLEX,					// a scrolly font
					sf,												// scale factor
					getTextColour(downloaded_image, word, sf),		// colour
					LINE_THICKNESS);								// thickness
		
		// construct the filename inclusing extension
		std::string filename(argv[ARGS::OUTPUT_NAME]);
		filename += ".jpg";
		
		std::cout << "Saving image to " << filename << std::endl;
		
		// save the image in the current folder
		cv::imwrite(filename, downloaded_image);
		all_done = true;
		
	}

	
	// ------- tidy up ------
	// no need to do anything to delete object etc
	// as i have used a smart pointer for it - arent i clever!
	// Still important to tell the user that it's done though
	if(all_done){
	std::cout << "Done, you " << word << std::endl;
	}
	else{
		std::cout << "Not done" << std::endl;
	}
	
}

//----------------------
// the word is going to go about a
// quarter of the way across the image
int getWordXPos(const int image_w){
	return image_w/4;
}

//----------------------
// the word is going to go about 3/4 of
// the way down the image
int getWordYPos(const int image_h){
	return (image_h/4)*3;
}

//----------------------
// Print the program usage
void printUsage(){
	std::cout << "Usage: ./swear_paper <output file name>" << std::endl;
}

//----------------------
// Looks at the area where the text is going to go and returns the
// colour code for black of white, depending on whether the area is lighter or darker
cv::Scalar getTextColour(const cv::Mat& downloaded_image, const std::string& word, const int sf){
	
	// the images' height and width
	int actual_h = downloaded_image.rows;
	int actual_w = downloaded_image.cols;
	
	// the location on the screen the text is going to go
	int x = getWordXPos(actual_w);		// a quarter across
	int y = getWordYPos(actual_h);	// three quarters down
	
	int word_w = getWordWidth(word, sf);
	int word_h = getWordHeight(word, sf);
	
	// Create a rectanlge big enough for the word and at the same x and y co-ords as it
	cv::Rect r(x, y, word_w, word_h);
	
	// Create a new matrix using only the part of the downloaded image covered by the rectanlge
	cv::Mat roi(downloaded_image(r));
	cv::Mat grey;
	
	// convert to grey and threshold, this will turn the lightest pixels white and the darker ones black
	cv::cvtColor(roi, grey, CV_BGR2GRAY);
	//cv::imshow("roi", roi);
	//cv::imshow("grey", grey);
	
	// anything brighter than half will remain the same, and anything less than 128 will fall to 0
	// this part is the bit which is abit dodgy.
	threshold(grey, grey, 128, 255, cv::THRESH_TOZERO);
	//cv::imshow("thres", grey);
	
	//cv::waitKey();
	
	// Get the number of pixels in the rectanlge, then a count of this fully off and those fully on
	int numwhite = cv::countNonZero(grey);
	int total = word_w * word_h;
	int numBlack = total - numwhite;
	
	// if there are more white pixels, then I want the text on the image to be black, to
	// stand out. similarly if more white i want the text to be black
	bool text_is_black = numwhite > numBlack;
	
	std::cout << "Text is " << (text_is_black ? "black" : "white") << std::endl;
	
	// return the rgb values for white or black
	return text_is_black ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
}

//----------------------
// Gets the word's width in pixels
int getWordWidth(const std::string& word, const int sf){
	return cv::getTextSize(word, CV_FONT_HERSHEY_SCRIPT_COMPLEX, sf, LINE_THICKNESS, nullptr).width;
}

//----------------------
// Gets the word height in pixels
int getWordHeight(const std::string& word, const int sf){
	return cv::getTextSize(word, CV_FONT_HERSHEY_SCRIPT_COMPLEX, sf, LINE_THICKNESS, nullptr).height;
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
		std::cout << e.what() << "\tline: " << __LINE__ << std::endl;
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
	int text_height = getWordHeight("f", 1);
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
