#ifndef __COLOR_MAP_HPP__
#define __COLOR_MAP_HPP__

//#include <tzk/opencv.h>
#include "gameCommon.h"
//#include "../include/tzk/opencv.h"

class MapFunc
{
public:
	CvScalar operator()(float r, float g, float b)
	{
		return doMap(b, g, r);
	}

	CvScalar virtual doMap(float r, float g, float b)
	{
		return cvScalar(b, g, r);
	}

	CvScalar mapColor(cv::Scalar color)
	{
		return doMap(color.val[0], color.val[1], color.val[2]);
	}

	cv::Mat mapDense(cv::Mat& src, cv::Mat& dst)
	{

	}
	void virtual setup(std::vector<CvScalar>& src, std::vector<CvScalar>& dst)
	{

	}
	void virtual print()
	{

	}
};

class LinearMapper : public MapFunc
{
public:
	void virtual setup(std::vector<CvScalar>& src, std::vector<CvScalar>& dst);
	CvScalar virtual doMap(float r, float g, float b);

	void virtual print();
	cv::Mat virtual mapDense(cv::Mat& src, cv::Mat& dst);
protected:
	cv::Mat mat;
};

class SimpleMapper : public MapFunc
{
public:
	void virtual setup(std::vector<CvScalar>& src, std::vector<CvScalar>& dst);

	CvScalar virtual doMap(float r, float g, float b);
	cv::Mat virtual mapDense(cv::Mat& src, cv::Mat& dst);
	void virtual print();
protected:
	cv::Scalar bias;
	cv::Scalar scale;
};

void colorThreshold(cv::Mat& color, cv::Mat& mask, float thresh);

class ColorMatcher
{
public:
	int debug;
	// param
	int ybins;
	int xbins;
	int DarkThresh;
	int FgThresh1;
	int FgThresh2;
	int BlurSize;
	float RectFgRatio;
	// member
	SimpleMapper lm;
	cv::Mat res;
	cv::Mat diff;
	cv::Mat mask;
	ColorMatcher();
	void update(cv::Mat& src, cv::Mat& dst);
	/* Return type of the rect: 0 - bk, 1 - fg
	 * @param rect Roi rect
	 * @param conf Confidence of the rect being fg
	 */
	int  getProb(cv::Rect rect, float* conf = NULL);
protected:
	void update1(cv::Mat& src, cv::Mat& dst);
};

void testMatchColor();

#endif /* __COLOR_MAP_HPP__ */