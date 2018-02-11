// Get the minimum values of each pixel among RGB channels 
// Written by Pengfei Peter Kan, Sep 17, 2017

#ifndef GetMinChn_H
#define GetMinChn_H

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

cv::Mat getMinChn(cv::Mat input_img)
{
	int chn_num = input_img.channels();
	cv::Mat min_chn = cv::Mat::zeros(input_img.size(), input_img.type());
	if (chn_num == 1)
	{
		min_chn = input_img;
	}
	else if (chn_num == 3)
	{
		std::vector<cv::Mat> chns(3);
		cv::split(input_img, chns);
		cv::min(chns[0], chns[1], min_chn);
		cv::min(min_chn, chns[2], min_chn);
	}
	return min_chn;
}
#endif // !GetMinChn_H

