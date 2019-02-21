#ifndef _DETECT_BASE__H__
#define _DETECT_BASE__H__
#include "cameraCelib.h"
//#include "colormap.hpp"
//#include "filter.h"
#include "motionDetection.h"

class detectBase {
public:
	detectBase();
	~detectBase();

	int  init(string  imgDir);

	int  destory();
	
	int  run(cv::Mat& nv21);

	int  getState();

	string  getImgPath();

	int  getRectInfo(cv::Rect& celibRect, vector<cv::Rect>& gameRects, vector<int>& gameResponds);

	void   detectTest();	

private:
	int   _inited;
	cv::Size  _imageSize;
	int _frameCounter;
	CCameraCelibBase  _celib;
	//ColorMatcher  _color;
	//detectFilter _filter;
	vector<cv::Point> _range;
	vector<cv::Rect> _celibRects;
	vector<cv::Rect> _gameRects;	
	vector<int> _gameRectsResponds;
	vector<string> _totalFiles;
	string  _imgPath;
	cv::Mat  _src;
	cv::Mat  _srcSmall, _camSmall;
	int  _switchImgStamp;
	cv::Mat  _mask;
	int  _respondCount;
	int  _changeSleepThresh;

	//��ȡ·���������ļ�
	void getFiles(string path, vector<string>& files);

	//�������������п�
	void  generateCelibRects(cv::Size& imgSize, vector<cv::Rect>& celibRects);

	//�������һ����
	cv::Rect  randRect(cv::Size& imgSize);

	//����һ����Ч��
	cv::Rect generateRandRect(cv::Size& imgSize, vector<cv::Point>& range, vector<cv::Rect>& rects, int type);

	//�ж�rect�Ƿ��ڶ�����ڲ�
	int rectInRange(cv::Rect& rect, vector<cv::Point>& range);

	//�ж�һ�����Ƿ����������ص�
	int  isOverlapRect(vector<cv::Rect>& rects, cv::Rect& curRect);

	int  correctCamera(cv::Mat& nv21);

	//ֱ���˶����
	CMotionDetectionBase  _motion;
};

#endif