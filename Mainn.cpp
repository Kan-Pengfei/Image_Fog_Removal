#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

#include "ImgFogRemoval.h"

int main()
{
	cv::Mat input_img = cv::imread("0.jpg");
	//ImgFogRemoval fog_img = ImgFogRemoval(input_img); // the defalt patch size is 15
	// declare the object with input patch size and eps
	ImgFogRemoval fog_img = ImgFogRemoval(input_img, 11, 0.001);
	cv::Mat origin_img = fog_img.getOriginImg();
	cv::imshow("The input image", origin_img);

	double sky_light = fog_img.estmSkyLight();
	std::cout << "The sky light of the image:" << sky_light << std::endl;

	cv::Mat dark_chn_img = fog_img.calcDarkChn();
	cv::imshow("The dark channel image", dark_chn_img);

	cv::Mat trans_M = fog_img.estmTransM();
	cv::imshow("The medium transmission matrix", trans_M);

	cv::Mat clear_img = fog_img.calcSceneRadiance();
	cv::imshow("The fog removed image", clear_img);

	cv::waitKey(0);
	return 0; 
}