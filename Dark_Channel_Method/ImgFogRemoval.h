// This is the image class for the fog removal image 
// all the image type is double 
// Written by Peter Kan, Jan 23, 2018
#ifndef ImgFogRemoval_H
#define ImgFogRemoval_H

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

class ImgFogRemoval
{
private:
	cv::Mat origin_img;		// original image with fog scene 
	// patch size for computing the dark channel image, and default value is 15x15
	int patch_size_v = 15; 	// value of the patch size 
	double eps = 0.001;		// regulation coefficeint for the guided filter 
	cv::Mat dark_chn_img; 
	double t0 = 0.01;		// t0 is the threshold that used to compute the scene radiance 
	cv::Mat trans_M; 
	cv::Mat clear_img; 

public:
	ImgFogRemoval(); 
	ImgFogRemoval(cv::Mat input_img);
	ImgFogRemoval(cv::Mat input_img, int input_size_v, double input_eps); 
	~ImgFogRemoval(); 
	// getter and setter functions 
	cv::Mat getOriginImg();
	void setOriginImg(cv::Mat input_img); 
	int getPatchSizeV(); // get the patch size value 
	void setPatchSizeV(int input_size_v); 
	double getEps(); 
	void setEps(double input_eps); 
	
	cv::Mat calcDarkChn();	// calculate the dark channel of the original image 
	double estmSkyLight(); // estimate the general sky light of the image 
	cv::Mat estmTransM();	// estimate the medium transmission matrix t(x)
	cv::Mat calcSceneRadiance(); // calculate the clear scene radiance of the fog image 
};

#endif // !ImgFogRemoval_H

