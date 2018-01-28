#ifndef My_Function_H
#define My_Function_H
// define my own general functions 

#include <opencv2\opencv.hpp>
#include <opencv2\highgui.hpp>

namespace MyFuncs
{
	class MyImgFuncs
	{
	public:
		static cv::Mat myConvertTo(cv::Mat input_img, std::string dst_type);
		static cv::Mat homoFilter(cv::Mat input_img);
	};
}
#endif // !My_Function_H

