#ifndef __MOTION_DETECT_H__
#define __MOTION_DETECT_H__

/************************************************************************
*	头文件
************************************************************************/
#include "gameCommon.h"
using namespace cv;
using namespace std;

#define  MOTION_DIFF_LEN   5

/************************************************************************
*	类定义
************************************************************************/
class CMotionDetectionBase
{
public:
	CMotionDetectionBase();
	~CMotionDetectionBase();

	int  run(cv::Mat& bgr);

	cv::Mat&  getMotionMask();

	void destroy();

	cv::Mat  _mergeMask;

private:	
	int  _frameCounter;
	int _curIdx;
	int  _postIdx;

	//图像尺寸
	std::vector<cv::Mat> _imgCash;
	cv::Mat  _mask;

	//将整理好的图像放入到cache中
	void  loadImg(cv::Mat& bgr);
};

#endif /* __MOTION_H__ */
