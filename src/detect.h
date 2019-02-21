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

	//获取路径下所有文件
	void getFiles(string path, vector<string>& files);

	//产生矫正的所有框
	void  generateCelibRects(cv::Size& imgSize, vector<cv::Rect>& celibRects);

	//随机生成一个框
	cv::Rect  randRect(cv::Size& imgSize);

	//生成一个有效框
	cv::Rect generateRandRect(cv::Size& imgSize, vector<cv::Point>& range, vector<cv::Rect>& rects, int type);

	//判断rect是否在多边形内部
	int rectInRange(cv::Rect& rect, vector<cv::Point>& range);

	//判断一个框是否与其它框重叠
	int  isOverlapRect(vector<cv::Rect>& rects, cv::Rect& curRect);

	int  correctCamera(cv::Mat& nv21);

	//直接运动检测
	CMotionDetectionBase  _motion;
};

#endif