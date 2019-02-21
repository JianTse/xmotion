#include "motionDetection.h"
#include <opencv2/highgui/highgui.hpp>

CMotionDetectionBase::CMotionDetectionBase()
{
	_frameCounter = 0;
	_curIdx = -1;
	_postIdx = -1;
}

CMotionDetectionBase::~CMotionDetectionBase()
{

}

void CMotionDetectionBase::destroy()
{
	_imgCash.clear();
	_frameCounter = 0;
	_curIdx = -1;
	_postIdx = -1;
}

void  CMotionDetectionBase::loadImg(cv::Mat& bgr)
{
	//判断和之前的图像size是否一样
	if (_imgCash.size() > 0)
	{
		if (bgr.cols != _imgCash[0].cols || bgr.rows != _imgCash[0].rows)
		{
			LOGI("img size change\n");
			destroy();
		}
	}	

	if (_imgCash.size() < MOTION_DIFF_LEN)
	{
		_imgCash.push_back(bgr);
		_curIdx = _imgCash.size() - 1;
	}
	else
	{
		_curIdx = _frameCounter % _imgCash.size();
		bgr.copyTo(_imgCash[_curIdx]);
	}	
	_postIdx = (_curIdx + 1) % _imgCash.size();	

	//mask
	if (_mask.empty())
	{
		_mask = cv::Mat::zeros(cv::Size(bgr.cols, bgr.rows), CV_8UC1);
		_mergeMask = _mask.clone();
	}
	else if (bgr.cols != _mask.cols || bgr.rows != _mask.rows)
	{
		_mask.release();
		_mask = cv::Mat::zeros(cv::Size(bgr.cols, bgr.rows), CV_8UC1);
		_mergeMask = _mask.clone();
	}
}

cv::Mat&  CMotionDetectionBase::getMotionMask()
{
	return _mask;
}

int  CMotionDetectionBase::run(cv::Mat& bgr)
{
	if (bgr.empty())
		return 0;	
	_frameCounter++;

	//读图像
	loadImg(bgr);

	//diff
	float ratio =	imgDiff(_imgCash[_curIdx], _imgCash[_postIdx], _mask, 12);	
	if (ratio > 0.4f)
		return 0;

	return 1;
}

