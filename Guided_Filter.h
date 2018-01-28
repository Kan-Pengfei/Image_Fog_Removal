#ifndef GUIDED_FILTER_H
#define GUIDED_FILTER_H

#include <opencv2/opencv.hpp>

class GuidedFilterImpl
{
public:
	virtual ~GuidedFilterImpl() {}

	cv::Mat filter(const cv::Mat &p, int depth);

protected:
	int Idepth;

private:
	virtual cv::Mat filterSingleChannel(const cv::Mat &p) const = 0;
};

// This class is for single channel images 
class GuidedFilterMono : public GuidedFilterImpl
{
public:
	GuidedFilterMono(const cv::Mat &I, int r, double eps);

private:
	virtual cv::Mat filterSingleChannel(const cv::Mat &p) const;

private:
	int r;
	double eps;
	cv::Mat I, mean_I, var_I;
};

// This class is for multiple channel images 
class GuidedFilterColor : public GuidedFilterImpl
{
public:
	GuidedFilterColor(const cv::Mat &I, int r, double eps);

private:
	virtual cv::Mat filterSingleChannel(const cv::Mat &p) const;

private:
	std::vector<cv::Mat> Ichannels;
	int r;
	double eps;
	cv::Mat mean_I_r, mean_I_g, mean_I_b;
	cv::Mat invrr, invrg, invrb, invgg, invgb, invbb;
};

class GuidedFilter
{
public:
	GuidedFilter(const cv::Mat &I, int r, double eps);
	~GuidedFilter();

	cv::Mat filter(const cv::Mat &p, int depth = -1) const;

private:
	GuidedFilterImpl *impl_;
};

// I is the guidance image, p is the input image for filtering, r is the radius and eps is regulation coefficient 
cv::Mat guidedFilter(const cv::Mat &I, const cv::Mat &p, int r, double eps, int depth = -1);

#endif
#pragma once
