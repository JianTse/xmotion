#include "gameCommon.h"

#define CLIP(lowBoundary, highBoundary, x)		((x)<(lowBoundary) ? (lowBoundary) : ((x)>(highBoundary) ? (highBoundary) : (x)))

#include <time.h>
#ifdef WIN32
#   include <windows.h>
#else
#   include <sys/time.h>
#endif
#ifdef WIN32
int
gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = (long)clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif

double   getTimeStamp()
{
#if defined WIN32 || defined _WIN32
	struct timeval     tv;
	gettimeofday(&tv, 0);
	double _windowsTime = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
	return _windowsTime;
#else
	struct timeval cur;
	gettimeofday(&cur, NULL);
	double _linuxTime = cur.tv_sec * 1000.0 + cur.tv_usec / 1000.0;
	return _linuxTime;
#endif
}

string find_replace(string str, string orig, string rep)
{
	int pos = str.find(orig);
	int n = orig.length();
	string substr = str.replace(pos, n, rep);
	return substr;
}

#if  0
void find_replace(char *str, char *orig, char *rep, char *output)
{
	char buffer[4096] = { 0 };
	char *p;

	sprintf(buffer, "%s", str);
	if (!(p = strstr(buffer, orig))){  // Is 'orig' even in 'str'?
		sprintf(output, "%s", str);
		return;
	}

	*p = '\0';

	sprintf(output, "%s%s%s", buffer, rep, p + strlen(orig));
}
#endif

void fnReduceNv12(cv::Mat& _src, cv::Mat& _dst, int _ratio)
{
	// Y size
	int srcWidth = _src.cols;
	int srcHeight = (_src.rows << 1) / 3;
	// UV size
	int srcHalfWidth = srcWidth >> 1;
	int srcHalfHeight = srcHeight >> 1;

	int doubleRatio = _ratio << 1;

	int dstWidth = _dst.cols;
	int dstHeight = (_dst.rows << 1) / 3;

	unsigned char *srcYPtr = _src.ptr<uchar>(0);
	unsigned char *srcUvPtr = (unsigned char*)(srcYPtr + _src.step * srcHeight);

	unsigned char *dstYPtr = _dst.ptr<uchar>(0);
	unsigned char *dstUvPtr = (unsigned char*)(dstYPtr + _dst.step * dstHeight);

	int srcYStep = _src.step * _ratio;
	int dstYStep = _dst.step - _dst.cols;

	int srcUvStep = _src.step * _ratio;
	int dstUvStep = _dst.step - _dst.cols;

	//Y
	for (int i = 0; i<srcHeight; i += _ratio)
	{
		for (int j = 0; j<srcWidth; j += _ratio)
		{
			*dstYPtr++ = srcYPtr[j];
		}

		srcYPtr += srcYStep;

		dstYPtr += dstYStep;
	}

	//UV
	for (int i = 0; i<srcHalfHeight; i += _ratio)
	{
		for (int j = 0; j<srcWidth; j += doubleRatio)
		{
			*dstUvPtr++ = srcUvPtr[j];

			*dstUvPtr++ = srcUvPtr[j + 1];
		}

		srcUvPtr += srcUvStep;

		dstUvPtr += dstUvStep;
	}
}

void Nv12ToRgb(cv::Mat& _rgb, cv::Mat&_yuv)
{
	int c, d, e;
	int r, g, b;
	int width = _rgb.cols;
	int height = _rgb.rows;

	unsigned char *yPtr1 = (unsigned char*)(_yuv.ptr<uchar>(0));
	unsigned char *yPtr2 = (unsigned char*)(yPtr1 + _yuv.step);
	unsigned char *uvPtr = (unsigned char*)(yPtr1 + _yuv.step * height);

	unsigned char *bmpPtr1 = (unsigned char*)(_rgb.ptr<uchar>(0));
	unsigned char *bmpPtr2 = (unsigned char*)(bmpPtr1 + _rgb.step);

	int yStep = (_yuv.step << 1) - (_yuv.cols * _yuv.channels());
	int uvStep = _yuv.step - (_yuv.cols * _yuv.channels());
	int bmpStep = (_rgb.step << 1) - (_rgb.cols * _rgb.channels());

	for (int i = 0; i<height; i += 2)
	{
		for (int j = 0; j<width; j += 2)
		{
#if 0
			d = (*uvPtr++) - 128;
			e = (*uvPtr++) - 128;
#else
			e = (*uvPtr++) - 128;
			d = (*uvPtr++) - 128;
#endif

			//y11
			c = (*yPtr1++) - 16;
			r = (298 * c + 409 * e + 128) >> 8;
			g = (298 * c - 100 * d - 208 * e + 128) >> 8;
			b = (298 * c + 516 * d + 128) >> 8;

			*bmpPtr1++ = CLIP(0, 255, b);
			*bmpPtr1++ = CLIP(0, 255, g);
			*bmpPtr1++ = CLIP(0, 255, r);

			//y12
			c = (*yPtr1++) - 16;
			r = (298 * c + 409 * e + 128) >> 8;
			g = (298 * c - 100 * d - 208 * e + 128) >> 8;
			b = (298 * c + 516 * d + 128) >> 8;

			*bmpPtr1++ = CLIP(0, 255, b);
			*bmpPtr1++ = CLIP(0, 255, g);
			*bmpPtr1++ = CLIP(0, 255, r);

			//y21
			c = (*yPtr2++) - 16;
			r = (298 * c + 409 * e + 128) >> 8;
			g = (298 * c - 100 * d - 208 * e + 128) >> 8;
			b = (298 * c + 516 * d + 128) >> 8;

			*bmpPtr2++ = CLIP(0, 255, b);
			*bmpPtr2++ = CLIP(0, 255, g);
			*bmpPtr2++ = CLIP(0, 255, r);

			//y22
			c = (*yPtr2++) - 16;
			r = (298 * c + 409 * e + 128) >> 8;
			g = (298 * c - 100 * d - 208 * e + 128) >> 8;
			b = (298 * c + 516 * d + 128) >> 8;

			*bmpPtr2++ = CLIP(0, 255, b);
			*bmpPtr2++ = CLIP(0, 255, g);
			*bmpPtr2++ = CLIP(0, 255, r);
		}

		bmpPtr1 += bmpStep;
		bmpPtr2 += bmpStep;
		yPtr1 += yStep;
		yPtr2 += yStep;
		uvPtr += uvStep;
	}
}

void RgbToNv12_ptr(unsigned char *_yuv, unsigned char *_raw, int _width, int _height)
{
	int r, g, b;
	int width = _width;
	int height = _height;
	int yuvWidthStep = width;
	int rawWidthStep = width * 3;

	unsigned char *yPtr1 = (unsigned char*)(_yuv);
	unsigned char *yPtr2 = (unsigned char*)(yPtr1 + yuvWidthStep);
	unsigned char *uvPtr = (unsigned char*)(yPtr1 + yuvWidthStep * height);

	unsigned char *bmpPtr1 = (unsigned char*)(_raw);
	unsigned char *bmpPtr2 = (unsigned char*)(bmpPtr1 + rawWidthStep);

	int yStep = (yuvWidthStep << 1) - _width;
	int uvStep = yuvWidthStep - _width;
	int bmpStep = (rawWidthStep << 1) - (_width * 3);

	for (int i = 0; i<height; i += 2)
	{
		for (int j = 0; j<width; j += 2)
		{
			//y11
			b = *bmpPtr1++;
			g = *bmpPtr1++;
			r = *bmpPtr1++;
			*yPtr1++ = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;

#if  0//USING_NV12_TYPE == NV12_TYPE_YUV
			*uvPtr++ = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
			*uvPtr++ = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
#else
			*uvPtr++ = ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;
			*uvPtr++ = ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
#endif

			//y12
			b = *bmpPtr1++;
			g = *bmpPtr1++;
			r = *bmpPtr1++;
			*yPtr1++ = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;

			//y21
			b = *bmpPtr2++;
			g = *bmpPtr2++;
			r = *bmpPtr2++;
			*yPtr2++ = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;

			//y22
			b = *bmpPtr2++;
			g = *bmpPtr2++;
			r = *bmpPtr2++;
			*yPtr2++ = ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
		}

		bmpPtr1 += bmpStep;
		bmpPtr2 += bmpStep;
		yPtr1 += yStep;
		yPtr2 += yStep;
		uvPtr += uvStep;
	}
}

cv::Point  getRectCenter(cv::Rect& rect)
{
	int x = rect.x + rect.width / 2.0f;
	int y = rect.y + rect.height / 2.0f;
	return cv::Point(x, y);
}

int  isValidRect(cv::Size& imgSize, cv::Rect& rect)
{
	int x1 = rect.x;
	int x2 = rect.x + rect.width;
	int y1 = rect.y;
	int y2 = rect.y + rect.height;
	if (x1 < 0 || x1 > imgSize.width - 1) return 0;
	if (x2 < 0 || x2 > imgSize.width - 1) return 0;
	if (y1 < 0 || y1 > imgSize.height - 1) return 0;
	if (y2 < 0 || y2 > imgSize.height - 1) return 0;
	return 1;
}

float getRectOverlap(cv::Rect _rect1, cv::Rect _rect2)
{
	if (_rect1.width <= 0 || _rect1.height <= 0 || _rect2.width <= 0 || _rect2.height <= 0)
	{
		return -1.0f;
	}

	float intersection, area1, area2;

	int overlapedWidth = MIN(_rect1.x + _rect1.width, _rect2.x + _rect2.width) - MAX(_rect1.x, _rect2.x);
	int overlapedHeight = MIN(_rect1.y + _rect1.height, _rect2.y + _rect2.height) - MAX(_rect1.y, _rect2.y);

	intersection = overlapedWidth * overlapedHeight;
	if (intersection <= 0 || overlapedWidth <= 0 || overlapedHeight <= 0)
		return 0.0f;

	area1 = _rect1.width * _rect1.height;
	area2 = _rect2.width * _rect2.height;

	return intersection / (area1 + area2 - intersection);
}

cv::Rect  covRect(cv::Rect& src, int scale)
{
	cv::Rect dst = src;
	dst.x = src.x / scale;
	dst.y = src.y / scale;
	dst.width = src.width / scale;
	dst.height = src.height / scale;
	return dst;
}

float imgDiff(cv::Mat& _img1, cv::Mat& _img2, cv::Mat& _mask, int _thresh)
{
	int  totalSum = 0;
	for (int y = 0; y < _img1.rows; y++)
	{
		uchar* ptrSrc1 = _img1.ptr<uchar>(y);
		uchar* ptrSrc2 = _img2.ptr<uchar>(y);
		uchar* ptrDst = _mask.ptr<uchar>(y);
		for (int x = 0; x < _img1.cols; x++)
		{
			ptrDst[x] = 0;
			uchar* ptrSrc11 = ptrSrc1 + x * _img1.channels();
			uchar* ptrSrc22 = ptrSrc2 + x * _img2.channels();
			for (int c = 0; c < _img1.channels(); c++)
			{
				int diff = abs(ptrSrc11[c] - ptrSrc22[c]);
				if (diff > _thresh)
				{
					ptrDst[x] = 255;
					totalSum++;
					break;
				}
			}
		}
	}

	//去燥
#if  0
	cv::blur(_mask, _mask, cv::Size(3, 3));
	cv::threshold(_mask, _mask, 200, 255, THRESH_BINARY);
#else
	Mat kern = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat erosion_img;
	erode(_mask, erosion_img, kern);
	dilate(erosion_img, _mask, kern);
#endif

	float  ratio = (float)totalSum / (float)(_mask.cols * _mask.rows);
	return ratio;
}

float  getMaskFrRatio(cv::Mat& mask, cv::Rect& rect)
{
	//统计非0像素个数
	int count = cv::countNonZero(mask(rect));
	float ratio = (float)count / (float)(rect.width * rect.height);
	return ratio;
}

float  getMatchTmpSim(cv::Mat& img1, cv::Rect& rect1, cv::Mat& img2, cv::Rect& rect2, int method)
{
	if (img1.empty() || img2.empty())
		return 0;
	if (rect1.width < 5 || rect1.height < 5)
		return 0;
	if (rect2.width < 5 || rect2.height < 5)
		return 0;
	cv::Mat patch1 = cv::Mat::zeros(cv::Size(32, 32), CV_8UC3);
	cv::Mat patch2 = cv::Mat::zeros(cv::Size(32, 32), CV_8UC3);
	cv::resize(img1(rect1), patch1, patch1.size());
	cv::resize(img2(rect2), patch2, patch2.size());
	Mat nccMat;
	matchTemplate(patch1, patch2, nccMat, method);    //TM_SQDIFF_NORMED    TM_CCOEFF_NORMED
	return nccMat.at<float>(0);
}

float imgsDiff(vector<cv::Mat>& _imgs, cv::Mat& _cur, cv::Mat& _mask, int _thresh)
{
	//把mask清零
	uchar* ptr = _mask.ptr<uchar>(0);
	memset(ptr, 0, _mask.cols*_mask.rows);

	int  totalSum = 0;
	for (int n = 0; n < _imgs.size(); n++)
	{
		for (int y = 0; y < _cur.rows; y++)
		{
			uchar* ptrSrc1 = _cur.ptr<uchar>(y);
			uchar* ptrSrc2 = _imgs[n].ptr<uchar>(y);
			uchar* ptrDst = _mask.ptr<uchar>(y);
			for (int x = 0; x < _cur.cols; x++)
			{
				uchar* ptrSrc11 = ptrSrc1 + x * _cur.channels();
				uchar* ptrSrc22 = ptrSrc2 + x * _imgs[n].channels();
				for (int c = 0; c < _cur.channels(); c++)
				{
					int diff = abs(ptrSrc11[c] - ptrSrc22[c]);
					if (diff > _thresh)
					{
						ptrDst[x] = 255;
						totalSum++;
						break;
					}
				}
			}
		}		
	}

	//去燥
	Mat kern = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat erosion_img;
	erode(_mask, erosion_img, kern);
	dilate(erosion_img, _mask, kern);

	float  ratio = (float)totalSum / (float)(_mask.cols * _mask.rows);
	return ratio;
}