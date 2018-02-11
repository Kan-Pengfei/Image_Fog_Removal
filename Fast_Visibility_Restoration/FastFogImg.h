// The is the class that implemented the fast visibility restoration method 
// Written by Pengfei Peter Kan in Jan 29, 2018
#ifndef FastFogImg_H
#define FastFogImg_H

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

class FastFogImg
{
private:
	cv::Mat origin_img; 
	int medium_filter_size = 41; // set the default value as 41
	double p = 0.95;	// the coefficient to estimate the atmospheric veil 
	cv::Mat atm_veil_img; 
	cv::Mat restored_scene_img; 
	cv::Mat tone_mapped_img; 

public:
	FastFogImg(){}
	FastFogImg(cv::Mat input_img); 
	FastFogImg(cv::Mat input_img, int input_filter_size); 
	~FastFogImg(); 

	// getter and setter functions 
	cv::Mat getOriginImg(); 
	int getMFilterSize(); 
	double getP(); 
	void setMFilterSize(int input_size); 

	cv::Mat getAtmsVeil();		// get the atmospheric veil of the image 
	cv::Mat restoreScene();		// restore the clear scene
	cv::Mat getToneMapped();	// tone map the restored image 
};

#endif // !FastFogImg_H

