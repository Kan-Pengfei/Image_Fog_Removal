#include "FastFogImg.h"
#include "Get_Min_Channel.h"
#include "Get_Mean_Channels.h"
#include "My_Functions.h"

FastFogImg::FastFogImg(cv::Mat input_img) : origin_img(input_img){}

FastFogImg::FastFogImg(cv::Mat input_img, int input_filter_size) :
	origin_img(input_img), medium_filter_size(input_filter_size) {}

FastFogImg::~FastFogImg() {}

// getter and setter functions 
cv::Mat FastFogImg::getOriginImg()
{
	return origin_img; 
}

int FastFogImg::getMFilterSize()
{
	return medium_filter_size; 
}

double FastFogImg::getP()
{
	return p; 
}

void FastFogImg::setMFilterSize(int input_size)
{
	medium_filter_size = input_size; 
}

// get the atmospheric veil of the image 
// V = max[min(p*B, W), 0], V is the atmosphere veil image 
// where W = min_c(I), I is the origin image
// B = A - median(|W - A|), A is the median blurred W
cv::Mat FastFogImg::getAtmsVeil()
{
	if (atm_veil_img.empty())
	{
		cv::Mat W = getMinChn(origin_img); // W = min_c{I(x,y)}
		cv::Mat A, B, C, D, E, F;

		cv::medianBlur(W, A, medium_filter_size); // medianBlur can only deal with CV_8U when size is large
		C = cv::abs(W - A);
		cv::medianBlur(C, D, medium_filter_size);
		B = A - D;
		// convert B, W to the double type 
		B.convertTo(B, CV_64F, 1.0 / 255.0);
		W.convertTo(W, CV_64F, 1.0 / 255.0);
		E = cv::min(B.mul(p), W);
		F = cv::Mat::zeros(E.size(), E.type());
		atm_veil_img = cv::max(E, F); // get the atmostphere veil 
	}
	return atm_veil_img; 
}

// restore the clear scene
// R = (I - V) / (1 - V/Is)
// where I is the original fog image, V is the atmospheric veil image 
cv::Mat FastFogImg::restoreScene()
{
	if (restored_scene_img.empty())
	{
		// convert origin image to double type image 
		cv::Mat origin_img_double;
		if (origin_img.type() % 8 == 0)
		{
			origin_img_double = MyFuncs::MyImgFuncs::myConvertTo(origin_img, "double");
		}
		cv::Mat V = getAtmsVeil();  // V has the double type 
		cv::Mat one_M = cv::Mat::ones(V.size(), V.type());
		cv::Mat G;
		cv::divide(one_M, one_M - V, G);
		int chn_num = origin_img_double.channels();
		if (chn_num == 1)
		{
			cv::multiply(origin_img_double - V, G, restored_scene_img);
		}
		else if (chn_num == 3)
		{
			std::vector<cv::Mat> origin_chns(3), R_chns(3);
			cv::split(origin_img_double, origin_chns);
			for (int c = 0; c < 3; ++c)
			{
				cv::multiply(origin_chns[c] - V, G, R_chns[c]);
			}
			cv::merge(R_chns, restored_scene_img);
		}
	}
	return restored_scene_img;
}

// tone map the restored image 
cv::Mat FastFogImg::getToneMapped()
{
	if (tone_mapped_img.empty())
	{
		cv::Mat restored_scene_img = restoreScene(); // get the restored clear scene of the image 
		cv::Mat I_mean = meanChannels(origin_img);  // I_mean and R_mean are double type 
		cv::Mat R_mean = meanChannels(restored_scene_img);
		// take the bottom 1/3 of the image to compute the mean and std deviation 
		cv::Mat roi_I = I_mean(cv::Rect(0, I_mean.rows * 2 / 3, I_mean.cols, I_mean.rows / 3));
		cv::Mat roi_R = R_mean(cv::Rect(0, R_mean.rows * 2 / 3, R_mean.cols, R_mean.rows / 3));
		cv::Mat Z = cv::Mat::ones(roi_I.size(), roi_I.type());
		// add tiny values to the roi_I and roi_R so that the log function will not return infinity
		roi_I = roi_I + Z.mul(0.5 / 255.0);
		roi_R = roi_R + Z.mul(0.5 / 255.0);
		cv::Mat I_log, R_log;
		cv::log(roi_I, I_log);
		cv::log(roi_R, R_log);
		// compute the mean and std of the reference image and the tone image in log space 
		cv::Mat AI, DI, AR, DR;
		double aI, dI, aR, dR;
		cv::meanStdDev(I_log, AI, DI);
		cv::meanStdDev(R_log, AR, DR);
		aI = AI.at<double>(0, 0);
		dI = DI.at<double>(0, 0);
		aR = AR.at<double>(0, 0);
		dR = DR.at<double>(0, 0);

		// compute U(x,y) = R(x,y)^(dI / dR) * exp(aI - aR * dI / dR)
		cv::Mat B, U, G;
		double e;
		cv::pow(restored_scene_img, dI / dR, B);
		e = std::exp(aI - aR * dI / dR);
		U = B.mul(e);

		// convert U to float so that can get the gray image of U
		// G is the gray image of U 
		U.convertTo(U, CV_32FC3);
		cv::cvtColor(U, G, CV_RGB2GRAY);
		// mG is the maximum of G
		double mG = 1;
		cv::minMaxIdx(G, NULL, &mG);
		// calculate the final toned image
		cv::Mat E = cv::Mat::ones(G.size(), G.type()) + G.mul(1 - 1 / mG);
		std::vector<cv::Mat> U_chns(3), T_chns(3);
		cv::split(U, U_chns);
		for (int c = 0; c < 3; ++c)
		{
			cv::divide(U_chns[c], E, T_chns[c]);
		}
		cv::merge(T_chns, tone_mapped_img);
	}
	return tone_mapped_img;
}