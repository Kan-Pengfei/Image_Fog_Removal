// The detail of this code is about the paper 
// Fast Visibility Restoration from a Single Color or Gray Level Image 
// by Jean-Philippe Tarel and NIcolas Hautiere 

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>
#include <time.h>

#include "FastFogImg.h"

int main()
{
	cv::Mat input_img = cv::imread("0.jpg"); 
	FastFogImg fog_img(input_img); 
	cv::Mat atm_veil_img = fog_img.getAtmsVeil(); 
	cv::imshow("The atm veil image", atm_veil_img); 
	cv::waitKey(10); 
	
	cv::Mat restored_scene = fog_img.restoreScene(); 
	cv::imshow("The restored scene", restored_scene); 
	cv::waitKey(10); 

	cv::Mat tone_mapped_img = fog_img.getToneMapped(); 
	cv::imshow("The tone mapped image", tone_mapped_img); 
	cv::waitKey(0); 
	return 0; 
}