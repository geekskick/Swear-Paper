//
//  image.cpp
//  Swear Paper
//


#include "image.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

image::image(void){
	init();
}

image::image(image& rhs){
	this->m_image = rhs.m_image ;
	this->m_line_thickness = rhs.m_line_thickness ;
	this->m_font = rhs.m_font ;
}

image::image(std::vector<char> & from){
	m_image = cv::imdecode(from, -1);
	init();
}

void image::put_text(const std::string& word){
	auto word_loc = word_location();
	cv::putText(m_image,
				word,
				cv::Point(word_loc.x,word_loc.y),
				CV_FONT_HERSHEY_SCRIPT_COMPLEX,
				scale_factor(),
				text_colour(word),
				m_line_thickness);
}

void image::save_to_file(const std::string& filename){
	cv::imwrite(filename, m_image);
}
 
bool image::word_fits(const std::string& word){
	struct size total_sz { word_location().x + text_size(word).w, word_location().y + text_size(word).h };
	return total_sz.w < m_image.cols && total_sz.h < m_image.rows;
}

struct size image::size(void){
	return { m_image.cols, m_image.rows };
}

cv::Scalar image::text_colour(const std::string& word){
	
	// the location on the screen the text is going to go
	struct location word_loc { word_location() };
	struct size word_size { text_size(word) };
	
	// Create a rectanlge big enough for the word and at the same x and y co-ords as it
	cv::Rect r(word_loc.x, word_loc.y, word_size.w, word_size.h);
	
	// Create a new matrix using only the part of the downloaded image covered by the rectanlge
	cv::Mat roi(m_image(r));
	cv::Mat grey;
	
	// convert to grey and threshold, this will turn the lightest pixels white and the darker ones black
	cv::cvtColor(roi, grey, CV_BGR2GRAY);
	
	// anything brighter than half will remain the same, and anything less than 128 will fall to 0
	// this part is the bit which is abit dodgy.
	threshold(grey, grey, 128, 255, cv::THRESH_TOZERO);
	
	// Get the number of pixels in the rectanlge, then a count of this fully off and those fully on
	int numwhite = cv::countNonZero(grey);
	int total = word_size.w * word_size.h;
	int numBlack = total - numwhite;
	
	// if there are more white pixels, then I want the text on the image to be black, to
	// stand out. similarly if more white i want the text to be black
	bool text_is_black = numwhite > numBlack;
	
	// return the rgb values for white or black
	return text_is_black ? cv::Scalar(0, 0, 0) : cv::Scalar(255, 255, 255);
}

int image::scale_factor(void){
	int text_height { cv::getTextSize("f", CV_FONT_HERSHEY_SCRIPT_COMPLEX, 1, m_line_thickness, nullptr).height };
	return double(m_image.rows / 20.0) / double(text_height);
}

struct location image::word_location(void){
	return { int(m_image.rows/4.0) , int((double(m_image.cols) * 3.0/4.0)) };
}

struct size image::text_size(const std::string& word){
	auto sz = cv::getTextSize(word, CV_FONT_HERSHEY_SCRIPT_COMPLEX, scale_factor(), m_line_thickness, nullptr);
	return { sz.width, sz.height };
}
void image::init(void){
	m_line_thickness = { 2 };
	m_font = { CV_FONT_HERSHEY_SCRIPT_COMPLEX };
}



