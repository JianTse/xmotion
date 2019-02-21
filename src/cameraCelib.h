#ifndef __CAMERA_CELIB_H__
#define __CAMERA_CELIB_H__

/************************************************************************
*	ͷ�ļ�
************************************************************************/
#include "gameCommon.h"
using namespace cv;
using namespace std;

/************************************************************************
*	�ඨ��
************************************************************************/
class CCameraCelibBase
{
public:
	CCameraCelibBase();
	~CCameraCelibBase();

	int  init(string dir);  //��ʼ��

	//���뱳��ͼ������ͷ�ɼ���nv21ͼ���������ͼ������ͷ�������С��bgrͼ
	void  getProcessImg(cv::Mat& srcBK, cv::Mat& srcNv21Cam, cv::Mat& dstBK, cv::Mat& dstBgrCam);

	// ��һ������ԭʼͼ��任��Сͼ��
	int  warpPoints(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts);
	int  warpPointsInv(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts);

	//���￪ʼ������
	int  getState();    //��ȡ�궨��״̬��ָ��android����
	int run(cv::Mat& img, cv::Rect& celibRect);		 //ִ�б궨
	int  getCelibRectId();  //��ȡ��������ţ�ָ��android����Ӧ�Ŀ�
	void  getMotionRange(vector<cv::Point>& range);   //��������ͷ�ܿ�������ͼƬ����Ч��Χ���ڱ���ͼ�еķ�Χ	
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

	//�任����
	cv::Mat  _warpMatrix;  
	cv::Mat  _warpMatrixInv;

	//����warp���camera	
	vector<cv::Point> _idx;
	uchar *  _smallNv21;
	int  _smallWidth;
	int  _smallHeight;
	void  initWarp();
	void  initWarpImgIdx(vector<cv::Point>& idx);

	int  clcWarpMatrix(cv::Rect& celibRect, int scale);  //����任����	
	int  clcMotionRange(cv::Rect& srcRect);  //��������Ч��Χ���ڱ���ͼ�е���Ч��Χ
	int  match(cv::Mat& gray, cv::Rect& srcRect);  //ƥ����̣����궨��

	void  saveMatchPts();
};

#endif /* __CAMERA_CELIB_H__ */
