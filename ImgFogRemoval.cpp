#include "ImgFogRemoval.h"
#include "Guided_Filter.h"
#include "My_Functions.h"

// constructors 
ImgFogRemoval::ImgFogRemoval() // default constructor 
{
	std::cout << "The originianl image is empty, you need to give an input iamge." << std::endl; 
}
ImgFogRemoval::ImgFogRemoval(cv::Mat input_img) : origin_img(input_img) 
{
	origin_img = MyFuncs::MyImgFuncs::myConvertTo(origin_img, "double"); // convert to double type 
}
ImgFogRemoval::ImgFogRemoval(cv::Mat input_img, int input_size_v, double input_eps)
	: origin_img(input_img), patch_size_v(input_size_v), eps(input_eps) 
{
	origin_img = MyFuncs::MyImgFuncs::myConvertTo(origin_img, "double"); // convert to double type 
}

ImgFogRemoval::~ImgFogRemoval() {}

// getter and setter functions for private variables 
cv::Mat ImgFogRemoval::getOriginImg()
{
	if (origin_img.empty())
	{
		exit(0); 
	}
	return origin_img; 
}
void ImgFogRemoval::setOriginImg(cv::Mat input_img)
{
	origin_img = input_img; 
}
int ImgFogRemoval::getPatchSizeV()
{
	return patch_size_v; 
}
void ImgFogRemoval::setPatchSizeV(int input_size_v)
{
	patch_size_v = input_size_v; 
}
double ImgFogRemoval::getEps()
{
	return eps; 
}
void ImgFogRemoval::setEps(double input_eps)
{
	eps = input_eps; 
}

// calculate the dark channel of the original image 
// which is the smallest intensity values inside the patch among all three channels 
cv::Mat ImgFogRemoval::calcDarkChn()
{
	// first find the smallest intensity channel values among RGB channels 
	// each pixel in one_chn_img has the smallest value among its previous RGB channels 
	cv::Mat one_ch_img = cv::Mat::ones(origin_img.size(), CV_64F);
	int ch_num = origin_img.channels();
	if (ch_num == 3)
	{
		// modify 3 channels into one channel by choosing the smallest intensity among all channels 
		std::vector<cv::Mat> channels(3);
		cv::split(origin_img, channels);
		cv::min(channels[0], channels[1], one_ch_img);
		cv::min(one_ch_img, channels[2], one_ch_img);
	}
	else  // the origin image is gray scale 
		one_ch_img = origin_img;
		
	// calculate the dark channel, whose pixels have the smallest intensity 
	// among all pixels in the corresponding patch 
	// extend the bountry of the clear image with patch size to make it easier to compute the dark channel 
	cv::Mat pre_img = cv::Mat::ones(origin_img.rows + patch_size_v, origin_img.cols + patch_size_v, CV_64F);
	cv::Mat embed_area(pre_img, cv::Rect(0, 0, one_ch_img.cols, one_ch_img.rows));
	one_ch_img.copyTo(embed_area); 

	dark_chn_img = cv::Mat::zeros(origin_img.size(), CV_64F);
	for (int i = 0; i < dark_chn_img.rows; ++i)
	{
		for (int j = 0; j < dark_chn_img.cols; ++j)
		{
			cv::Rect roi = cv::Rect(j, i, patch_size_v, patch_size_v); // watch out the rect uses x, y 
			cv::Mat roi_img = pre_img(roi);
			double min_inten; 
			cv::minMaxIdx(roi_img, &min_inten); // get the minimum intensity in the patch 
			dark_chn_img.at<double>(i, j) = min_inten; // at uses row and col 
		}
	}
	return dark_chn_img;
}

// chooose the largest intensity of the dark channel image as the general sky light of the original image 
double ImgFogRemoval::estmSkyLight()
{
	if (dark_chn_img.empty())
		dark_chn_img = calcDarkChn(); 
	double sky_light; 
	cv::minMaxIdx(dark_chn_img, NULL, &sky_light); 
	return sky_light; // adjust the sky light to get better result 
}

// apply the guided filter to estimate the medium transmission matrix t(x)
cv::Mat ImgFogRemoval::estmTransM()
{
	if (dark_chn_img.empty())
		dark_chn_img = calcDarkChn();
	cv::Mat I = cv::Mat::ones(dark_chn_img.size(), dark_chn_img.type());
	cv::Mat estm_trans_M = I - dark_chn_img;
	// apply guided filter here to do the edge preserving smoothing and get t(x)
	// take the input image as the reference and the patch size as the filter size 
	cv::Mat trans_M = guidedFilter(origin_img, estm_trans_M, patch_size_v, eps, -1);
	return trans_M;
}

// calculate the clear radiance scene of the fog image 
// based on J = (I - A)/max(t, t0) + A
cv::Mat ImgFogRemoval::calcSceneRadiance()
{
	if (trans_M.empty())
		trans_M = estmTransM(); 
	double sky_light = estmSkyLight(); 
	std::vector<cv::Mat>origin_chs(3);
	cv::split(origin_img, origin_chs);
	std::vector<cv::Mat> scene_radiance_chns(3);
	for (int n = 0; n < 3; ++n)
	{
		scene_radiance_chns[n] = cv::Mat::zeros(origin_img.size(), CV_64F);
		for (int i = 0; i < scene_radiance_chns[n].rows; ++i)
		{
			for (int j = 0; j < scene_radiance_chns[n].cols; ++j)
			{
				// double t = input_image.medium_trans_estimate.at<double>(i, j); 
				double t = trans_M.at<double>(i, j);
				if (t < t0) 
					t = t0;
				double a1 = origin_chs[n].at <double>(i, j);
				double intensity = (a1 - sky_light) / t + sky_light;
				if (intensity > 1.0)
					intensity = 1.0;
				scene_radiance_chns[n].at<double>(i, j) = intensity;
			}
		}
	}
	cv::Mat scene_radiance;
	cv::merge(scene_radiance_chns, scene_radiance);
	return scene_radiance;
}