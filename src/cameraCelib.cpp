#include "cameraCelib.h"

CCameraCelibBase::CCameraCelibBase()
{
	_imgSize = cv::Size(0,0);
	_curState = camera_state::estable_bk;
	_rectIdx = 0;
	_bkFrameCounter = 0;
	_startMatchCounter = 0;
	_frameCounter = 0;	
	_smallNv21 = NULL;
	_smallWidth = 0;
	_smallHeight = 0;
}

CCameraCelibBase::~CCameraCelibBase()
{
	if (_smallNv21 != NULL)
	{
		delete[] _smallNv21;
		_smallNv21 = NULL;
	}
}

void CCameraCelibBase::destroy()
{
	_curState = camera_state::estable_bk;
	_srcPts.clear();
	_dstPts.clear();
	_idx.clear();
	_rectIdx = 0;
	_bkFrameCounter = 0;
	_startMatchCounter = 0;
	_frameCounter = 0;
	_warpMatrix.release();
	_warpMatrixInv.release();
	if (_smallNv21 != NULL)
	{
		delete[] _smallNv21;
		_smallNv21 = NULL;
	}
}

int  CCameraCelibBase::init(string dir)
{	
	_matchFn = dir + "/match.txt";
	FILE* fp = fopen(_matchFn.c_str(), "r");
	if (fp == NULL)
		return 0;

	_srcPts.clear();
	_dstPts.clear();
	int  x1, y1, x2, y2;
	while (true) {
		const int status = fscanf(fp, "%d,%d,%d,%d\n",
			&x1, &y1, &x2, &y2);
		if (status == EOF) {
			break;
		}
		_srcPts.push_back(cv::Point(x1,y1));
		_dstPts.push_back(cv::Point(x2, y2));
	}
	fclose(fp);

	//计算变换矩阵
	initWarp();

	//如果读进来，则认为矫正完成了
	_curState = camera_state::celib_over;
	return 1;
}

void  CCameraCelibBase::initWarp()
{
	_smallWidth = bigWidth / SCALE;
	_smallHeight = bigHeight / SCALE;

	float x, y;
	vector<cv::Point2f> _scaleSrcPts;
	vector<cv::Point2f> _scaleDstPts;
	for (int i = 0; i < _dstPts.size(); i++)
	{
		x = _srcPts[i].x / SCALE;
		y = _srcPts[i].y / SCALE;
		_scaleSrcPts.push_back(cv::Point2f(x, y));

		x = _dstPts[i].x;
		y = _dstPts[i].y;
		_scaleDstPts.push_back(cv::Point2f(x, y));
	}
	_warpMatrix = cv::findHomography(_scaleSrcPts, _scaleDstPts);
	_warpMatrixInv = cv::findHomography(_scaleDstPts, _scaleSrcPts);

	//计算将大的yuv warp到小yuv的索引
	initWarpImgIdx(_idx);	

	//创建小nv21	
	int smallNv21Size = _smallWidth*_smallHeight + (_smallWidth * _smallHeight) / 2;
	if (_smallNv21)
	{
		delete[] _smallNv21;
	}
	_smallNv21 = new uchar[smallNv21Size];
	memset(_smallNv21, 0, smallNv21Size);

	//找出原始图像中的有效区域
	cv::Rect rect = cv::Rect(0, 0, 0, 0);
	clcMotionRange(rect);
}

int  CCameraCelibBase::match(cv::Mat& bgr, cv::Rect& srcRect)
{
	if (srcRect.width < 1 || srcRect.height < 1)
		return 0;
	if (_curState != camera_state::start_match)
		return 0;

	imgDiff(bgr, _background, _mask, 16);

	//cv::imwrite("./sdcard/DCIM/xj/test.jpg", _mask);

	//查找所有外轮廓
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(_mask, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	//遍历每个轮廓，当长宽比，面积都满足一定阈值则成功
	int bestId = -1;
	int findCount = 0;
	float  imgSize = bgr.cols * bgr.rows / 100;
	float minSizeThresh = max(10.0f, (float)imgSize / (float)(2.25f ));
	float maxSizeThresh = max(10.0f, (float)imgSize * 2.25f );
	float imgWh = (float)bgr.cols / (float)bgr.rows;
	float minWHThresh = (float)imgWh / 1.5f;
	float maxWHThresh = (float)imgWh * 1.5f;
	for (int i = 0; i < contours.size(); i++)
	{
		float area = contourArea(contours[i]);
		cv::Rect boundRect = cv::boundingRect(contours[i]);
		float wh_ratio = (float)boundRect.width / (float)boundRect.height;
		if (area > minSizeThresh && area < maxSizeThresh && wh_ratio > minWHThresh && wh_ratio < maxWHThresh)
		{
			//保存这对匹配点对
			bestId = i;
			findCount++;
		}
	}

	if (bestId >= 0 && findCount == 1)
	{				
		_curState = camera_state::end_match;
		cv::Rect boundRect = cv::boundingRect(contours[bestId]);
		_srcPts.push_back(getRectCenter(srcRect));
		_dstPts.push_back(getRectCenter(boundRect));
		//drawContours(maskClone, contours, bestId, cv::Scalar(255), 2, 8, hierarchy, 0, Point());
		LOGI("matched: %d", _dstPts.size());
	}

	//cv::imshow("counter", maskClone);
	return findCount;
}

int  CCameraCelibBase::getState()
{
	return _curState;
}

int  CCameraCelibBase::clcMotionRange(cv::Rect& srcRect)
{
	if (srcRect.width > 1 || srcRect.height > 1)
	{
		return 0;
	}	
	if (_dstPts.size() != _srcPts.size())
	{
		LOGI("not match size\n");
		return -1;
	}
	if (_dstPts.size() < 5)
	{
		LOGI("pair match too small: %d\n", _dstPts.size());
		return -1;
	}		

	//计算变换矩阵
	cv::Mat H = cv::findHomography(_srcPts, _dstPts);
	cv::Mat H_Inv = cv::findHomography(_dstPts, _srcPts);

	//根据变化矩阵计算区域范围
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(bigWidth-1, 0);
	obj_corners[2] = cvPoint(bigWidth-1, bigHeight-1);
	obj_corners[3] = cvPoint(0, bigHeight-1);
	dst_range.resize(4);
	cv::perspectiveTransform(obj_corners, dst_range, H);

	//限制在有效范围内
	for (int i = 0; i < dst_range.size(); i++)
	{
		if (dst_range[i].x < 0) dst_range[i].x = 0;
		if (dst_range[i].x > bigWidth - 1) dst_range[i].x = bigWidth - 1;
		if (dst_range[i].y < 0) dst_range[i].y = 0;
		if (dst_range[i].y > bigHeight - 1) dst_range[i].y = bigHeight - 1;
	}

	//然后在反映射到原始图像中的范围
	src_range.resize(4);
	cv::perspectiveTransform(dst_range, src_range, H_Inv);

	//判断矫正的范围是否满足要求，否则继续矫正
	int  isRight = 1;
	bool ret = cv::isContourConvex(src_range);
	if (!ret)  //说明不是凸多边形
	{
		isRight = -1;
		LOGI("is not convex\n");
		return -1;
	}
	double  area = cv::contourArea(src_range);
	double  thresh = (bigWidth * bigHeight);
	if (area < thresh * 0.4)  //说明面积不合适
	{
		isRight = -1;
		LOGI("area is small\n");
		return -1;
	}
	return isRight;
}

int  CCameraCelibBase::run(cv::Mat& img, cv::Rect& celibRect)
{
	_frameCounter++;
	_imgSize.width = img.cols;
	_imgSize.height = img.rows * 2 / 3;
	//当传进来的rect无效时，则认为矫正完毕
	if (_curState == camera_state::celib_over)
	{
		return _curState;
	}
	if (isValidRect(_imgSize, celibRect) == 0)
	{
		_curState = camera_state::end_match;
		_rectIdx = _rectIdx + 1;
		return  _curState;
	}

	//将原始图像转换为rgb图像
	if (_bgr.empty())
	{
		_bgr = cv::Mat::zeros(_imgSize.height, _imgSize.width, CV_8UC3);
		_mask = cv::Mat::zeros(_imgSize.height, _imgSize.width, CV_8UC1);
	}
	if (_bgr.cols != _imgSize.width || _bgr.rows != _imgSize.height)
	{
		_bgr.release();
		_mask.release();
		_bgr = cv::Mat::zeros(_imgSize.height, _imgSize.width, CV_8UC3);
		_mask = cv::Mat::zeros(_imgSize.height, _imgSize.width, CV_8UC1);
	}
	Nv12ToRgb(_bgr, img);

	//创建背景图像
	if (_background.empty())
	{
		_background = _bgr.clone();
		_curState = camera_state::estable_bk;
	}
	if (_background.cols != _bgr.cols || _background.rows != _bgr.rows)
	{
		_background = _bgr.clone();
		_curState = camera_state::estable_bk;
	}

	//执行流程
	switch (_curState)
	{
	case  camera_state::estable_bk:		  //创建背景
		if (_bkFrameCounter < 10)
		{
			_bkFrameCounter++;
			_background = _bgr.clone();
		}
		else
		{
			_curState = camera_state::start_match;
			_bkFrameCounter = 0;
		}
		break;
	case  camera_state::start_match:   //开始匹配
		if (_startMatchCounter < 8)
		{
			_startMatchCounter++;
			if (_startMatchCounter > 5)
			{
				int ret = match(_bgr, celibRect);
				if (ret == 1)
				{
					_curState = camera_state::end_match;
					_startMatchCounter = 0;
				}
			}
		}
		else
		{
			_curState = camera_state::end_match;
			_startMatchCounter = 0;
		}		
		break;
	case  camera_state::end_match:  //指向下一个矫正块
		_rectIdx = _rectIdx + 1;
		_curState = camera_state::estable_bk;		
		break;
	default:
		break;
	}

	LOGI("matched: %d, rectId: %d", _dstPts.size(), _rectIdx);

	//如果定位完，则计算透视变换矩阵
	int ret = clcMotionRange(celibRect);
	if (ret == 1)   //如果合适，则返回矫正完成
	{
		initWarp();
		_curState = camera_state::celib_over;
		saveMatchPts();
	}
	else if (ret == -1)  //如果该变换不对，则重新搞
	{
		destroy();
	}

	return _curState;
}

int  CCameraCelibBase::getCelibRectId()
{
	return _rectIdx;
}

void  CCameraCelibBase::getMotionRange(vector<cv::Point>& range)
{
	range.clear();	
	for (int i = 0; i < src_range.size(); i++)
	{
		int x = cvRound(src_range[i].x);
		int y = cvRound(src_range[i].y);
		range.push_back(cv::Point(x, y));
	}
}

int  CCameraCelibBase::warpPoints(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts)
{
	if (_dstPts.size() != _srcPts.size())
	{
		LOGI("not match size\n");
		return 0;
	}
	if (_dstPts.size() < 5)
	{
		LOGI("pair match too small: %d\n", _dstPts.size());
		return 0;
	}

	dstPts.resize(srcPts.size());
	cv::perspectiveTransform(srcPts, dstPts, _warpMatrix);
	return 1;
}

int  CCameraCelibBase::warpPointsInv(vector<cv::Point2f>& srcPts, vector<cv::Point2f>& dstPts)
{
	if (_dstPts.size() != _srcPts.size())
	{
		LOGI("not match size\n");
		return 0;
	}
	if (_dstPts.size() < 5)
	{
		LOGI("pair match too small: %d\n", _dstPts.size());
		return 0;
	}

	dstPts.resize(srcPts.size());
	cv::perspectiveTransform(srcPts, dstPts, _warpMatrixInv);
	return 1;
}

void  CCameraCelibBase::initWarpImgIdx(vector<cv::Point>& idx)
{	
	//warp一个点从原始图像到目的图像
	vector<cv::Point2f> rgbIdx;
	vector<cv::Point2f> yuvIdx;
	for (int y = 0; y < _smallHeight; y++)
	{
		for (int x = 0; x < _smallWidth; x++)
		{
			rgbIdx.push_back(cv::Point2f(x, y));
		}
	}
	warpPoints(rgbIdx, yuvIdx);

	//根据yIdx计算uvIdx
	for (int y = 0; y < _smallHeight; y += 2)
	{
		for (int x = 0; x < _smallWidth; x += 2)
		{
			//y分量在原始yuv中的索引
			int idx = y * _smallWidth + x;    //在yuvIdx中y的索引
			int raw_idx_x = cvRound(yuvIdx[idx].x);  //在原始nv21中对应的x坐标
			int raw_idx_y = cvRound(yuvIdx[idx].y);  //在原始nv21中对应的y坐标

			//与y对应的uv在yuv中的索引
			int raw_idx_u_x = (raw_idx_x / 2) * 2;
			int raw_idx_u_y = bigHeight + raw_idx_y / 2;
			yuvIdx.push_back(cv::Point2f(raw_idx_u_x, raw_idx_u_y));

			int raw_idx_v_x = raw_idx_u_x + 1;
			int raw_idx_v_y = raw_idx_u_y;
			yuvIdx.push_back(cv::Point2f(raw_idx_v_x, raw_idx_v_y));
		}
	}

	idx.clear();
	for (int i = 0; i < yuvIdx.size(); i++)
	{
		int  y = cvRound(yuvIdx[i].y);
		int  x = cvRound(yuvIdx[i].x);
		idx.push_back(cv::Point(x,y));
	}
}

void  CCameraCelibBase::getProcessImg(cv::Mat& srcBK, cv::Mat& srcNv21Cam, cv::Mat& dstBK, cv::Mat& dstBgrCam)
{
	//double  t1 = getTimeStamp();
	//cv::resize(srcBK, dstBK, cv::Size(_smallWidth, _smallHeight));
	//double  t2 = getTimeStamp();
	//直接从大nv21中提取warp后的小的nv21并转换为bgr
	int smallNv21Size = _smallWidth*_smallHeight + (_smallWidth * _smallHeight) / 2;
	for (int i = 0; i < smallNv21Size; i++)
	{
		int y = _idx[i].y;
		int x = _idx[i].x;
		if (y < 0 || y >= bigHeight*3/2 || x < 0 || x >= bigWidth)
			continue;

		_smallNv21[i] = srcNv21Cam.at<uchar>(_idx[i].y, _idx[i].x);
	}
	cv::Mat  nv21Small(_smallHeight * 3 / 2, _smallWidth, CV_8UC1, _smallNv21);
	cv::Mat dst = cv::Mat::zeros(cv::Size(_smallWidth, _smallHeight), CV_8UC3);
	Nv12ToRgb(dst, nv21Small);
	dstBgrCam = dst.clone();
	//double  t3 = getTimeStamp();
	//LOGI("warp time: %.2f,%.2f", t2-t1,t3-t1);
}

void  CCameraCelibBase::saveMatchPts()
{
	if (_curState != camera_state::celib_over)
	{
		LOGI("_curState: %d\n", _curState);
		return;
	}

	LOGI("save match pts\n");
	FILE* dst_file_ptr = fopen(_matchFn.c_str(), "w");
	for (int i = 0; i < _dstPts.size(); i++)
	{
		fprintf(dst_file_ptr, "%d,%d,%d,%d\n", cvRound(_srcPts[i].x), cvRound(_srcPts[i].y), cvRound(_dstPts[i].x), cvRound(_dstPts[i].y));
		fflush(dst_file_ptr);
	}
	fclose(dst_file_ptr);
}

void  CCameraCelibBase::test()
{
#if  0
	char* config = "E:/work/testData/image/match/xj_person/match.txt";
	char* srcFn = "E:/work/testData/image/match/src/5.jpg";
	char* dstFn = "E:/work/testData/image/match/xj_person/5.jpg";
	init(config);  //初始化

	cv::Mat src = imread(srcFn);
	if (src.empty()) return;

	cv::Mat dst = imread(dstFn);
	if (dst.empty()) return;

	//提取camera的nv21数据
	int bigNv21Size = dst.cols*dst.rows + (dst.cols * dst.rows) / 2;
	uchar *  bigNv21 = new uchar[bigNv21Size];
	RgbToNv12_ptr(bigNv21, dst.data, dst.cols, dst.rows);
	cv::Mat  nv21Big(dst.rows * 3 / 2, dst.cols, CV_8UC1, bigNv21);
	
	
	cv::Mat dstBK, dstBgrCm;
	getProcessImg(src, nv21Big, dstBK, dstBgrCm);

	imshow("nv21Big", nv21Big);
	imshow("srcBK", src);
	imshow("smallBK", dstBK);
	imshow("smallCam", dstBgrCm);
	cv::waitKey(0);

	delete[] bigNv21;	
#endif
}



