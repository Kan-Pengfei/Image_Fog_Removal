// Take the mean value between channels 
// The output is one channel image with type of CV_64F
// Every pixel has the intensity of 
// the mean value of the corresponding pixel in RGB channels
// Written by Pengfei Peter Kan, Sep 17, 2017

#ifndef getMeanChn_H
#define getMeanChn_H

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

#include <vector>

cv::Mat meanChannels(cv::Mat input_img)
{
	int chn_num = input_img.channels();
	cv::Mat mean_img;
	if (chn_num == 1)
	{
		if (input_img.type() % 8 == 0) // unchar type image has type value of 8, 16, 24 
		{
			input_img.convertTo(input_img, CV_64F, 1.0 / 255.0);
		}
		mean_img = input_img;
	}
	else if (chn_num == 3)
	{
		if (input_img.type() % 8 == 0) 
		{
			input_img.convertTo(input_img, CV_64FC3, 1.0 / 255.0);
		}
		mean_img = cv::Mat::zeros(input_img.size(), CV_64F);
		for (int row = 0; row < input_img.rows; ++row)
		{
			for (int col = 0; col < input_img.cols; ++col)
			{
				cv::Vec3d inten = input_img.at<cv::Vec3d>(row, col);
				// calculate the mean of intensities in three channels 
				mean_img.at<double>(row, col) = (inten[0] + inten[1] + inten[2]) / 3.0;
			}
		}
	}
	return mean_img;
}
#endif // !getMeanChn_H

