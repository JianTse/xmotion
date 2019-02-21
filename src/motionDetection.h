#ifndef __MOTION_DETECT_H__
#define __MOTION_DETECT_H__

/************************************************************************
*	ͷ�ļ�
************************************************************************/
#include "gameCommon.h"
using namespace cv;
using namespace std;

#define  MOTION_DIFF_LEN   5

/************************************************************************
*	�ඨ��
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

	//ͼ��ߴ�
	std::vector<cv::Mat> _imgCash;
	cv::Mat  _mask;

	//������õ�ͼ����뵽cache��
	void  loadImg(cv::Mat& bgr);
};

#endif /* __MOTION_H__ */
