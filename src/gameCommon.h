#ifndef __MOTION_COMMON_H__
#define __MOTION_COMMON_H__

/************************************************************************
*	Í·ÎÄ¼þ
************************************************************************/
#ifdef WIN32
#include <tzk/opencv.h>
#else
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp> 
#include <opencv2/imgproc/imgproc.hpp>
#endif
#include "androidlog.h"
#include <vector>
#include<iostream>
#include<string>
using namespace cv;
using namespace std;

#define  bigWidth  1280
#define  bigHeight  720
#define  SCALE   4

#define BIN_B 16
#define BIN_G 16
#define BIN_R 16
#define  HIST_LEN   BIN_B * BIN_G * BIN_R

enum rectState
{
	unknow = 0,
	back,
	fore
};

enum camera_state
{
	estable_bk = 2,
	start_match,
	end_match,
	pts_finish,
	celib_over
};

double   getTimeStamp();
//void find_replace(char *str, char *orig, char *rep, char *output);
string find_replace(string str, string orig, string rep);
void fnReduceNv12(cv::Mat& _src, cv::Mat& _dst, int _ratio);
void Nv12ToRgb(cv::Mat& _rgb, cv::Mat&_yuv);
void RgbToNv12_ptr(unsigned char *_yuv, unsigned char *_raw, int _width, int _height);
cv::Point  getRectCenter(cv::Rect& rect);
int  isValidRect(cv::Size& imgSize, cv::Rect& rect);
float getRectOverlap(cv::Rect _rect1, cv::Rect _rect2);
cv::Rect  covRect(cv::Rect& src, int scale);
float imgDiff(cv::Mat& _img1, cv::Mat& _img2, cv::Mat& _mask, int _thresh);
float  getMatchTmpSim(cv::Mat& img1, cv::Rect& rect1, cv::Mat& img2, cv::Rect& rect2, int method);
float  getMaskFrRatio(cv::Mat& mask, cv::Rect& rect);
float imgsDiff(vector<cv::Mat>& _imgs, cv::Mat& _cur, cv::Mat& _mask, int _thresh);

#endif /* __CAMERA_CELIB_H__ */
