#ifndef __CAMERA_CELIB_H__
#define __CAMERA_CELIB_H__

/************************************************************************
*	头文件
************************************************************************/
#include "gameCommon.h"
using namespace cv;
using namespace std;

/************************************************************************
*	类定义
************************************************************************/
class CCameraCelibBase
{
public:
	CCameraCelibBase();
	~CCameraCelibBase();

	int  init(string dir);  //初始化

	//输入背景图、摄像头采集的nv21图，输出背景图与摄像头对齐的缩小的bgr图
	void  getProcessImg(cv::Mat& srcBK, cv::Mat& srcNv21Cam, cv::Mat& dstBK, cv::Mat& dstBgrCam);

	// 将一个点有原始图像变换到小图像
	int  warpPoints(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts);
	int  warpPointsInv(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts);

	//这里开始做矫正
	int  getState();    //获取标定的状态，指导android流程
	int run(cv::Mat& img, cv::Rect& celibRect);		 //执行标定
	int  getCelibRectId();  //获取定标框的序号，指导android画对应的框
	void  getMotionRange(vector<cv::Point>& range);   //计算摄像头能看到给出图片的有效范围，在背景图中的范围	
	void destroy();

	void  test();
private:	
	int   _curState;
	int  _frameCounter;
	cv::Size  _imgSize;
	cv::Mat  _bgr;
	cv::Mat  _background;
	cv::Mat  _mask;
	int  _rectIdx;
	int  _bkFrameCounter;
	int  _startMatchCounter;	
	string _matchFn;
	
	vector<Point2f> src_range;
	vector<Point2f> dst_range;
	vector<cv::Point2f> _srcPts;
	vector<cv::Point2f> _dstPts;

	//变换矩阵
	cv::Mat  _warpMatrix;  
	cv::Mat  _warpMatrixInv;

	//计算warp后的camera	
	vector<cv::Point> _idx;
	uchar *  _smallNv21;
	int  _smallWidth;
	int  _smallHeight;
	void  initWarp();
	void  initWarpImgIdx(vector<cv::Point>& idx);

	int  clcWarpMatrix(cv::Rect& celibRect, int scale);  //计算变换矩阵	
	int  clcMotionRange(cv::Rect& srcRect);  //则计算出有效范围，在背景图中的有效范围
	int  match(cv::Mat& gray, cv::Rect& srcRect);  //匹配过程，检测标定框

	void  saveMatchPts();
};

#endif /* __CAMERA_CELIB_H__ */
