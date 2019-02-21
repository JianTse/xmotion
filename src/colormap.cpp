#include "colormap.hpp"
#include <vector>
#include <time.h>

using namespace cv;

void LinearMapper::setup(std::vector<CvScalar>& src, std::vector<CvScalar>& dst)
{
	int N = src.size();
	cv::Mat A(N, 4, CV_32F);
	cv::Mat B(N, 3, CV_32F);
	for (int i = 0; i < src.size(); i++)
	{
		int row = i;
		A.at<float>(row, 0) = 1;
		A.at<float>(row, 1) = src[i].val[0];
		A.at<float>(row, 2) = src[i].val[1];
		A.at<float>(row, 3) = src[i].val[2];
		B.at<float>(row, 0) = dst[i].val[0];
		B.at<float>(row, 1) = dst[i].val[1];
		B.at<float>(row, 2) = dst[i].val[2];
	}

	cv::Mat X;
	cv::solve(A, B, X, DECOMP_SVD | DECOMP_NORMAL);
	cv::Mat E = A * X - B;
	mat = X;
	// report fitting result
	//print();
}
CvScalar LinearMapper::doMap(float r, float g, float b)
{
	cv::Mat A(1, 4, CV_32F);
	int row = 0;
	A.at<float>(row, 0) = 1;
	A.at<float>(row, 1) = b;
	A.at<float>(row, 2) = g;
	A.at<float>(row, 3) = r;
	cv::Mat D = A * mat;
	return cvScalar(D.at<float>(0, 0), D.at<float>(0, 1), D.at<float>(0, 2));
}

void LinearMapper::print()
{
	printf("\nmean:");
	for (int i = 0; i < 3; i++)
	{
		printf("%.3f ", mat.at<float>(0, i));
	}
	printf("\n");
	char chs[3] = { 'b', 'g', 'r' };
	for (int i = 0; i < 3; i++)
	{
		printf("%c   :", chs[i]);
		for (size_t j = 0; j < 3; j++)
		{
			printf("%.3f ", mat.at<float>(j + 1, i));
		}
		printf("\n");
	}
}
cv::Mat LinearMapper::mapDense(cv::Mat& src, cv::Mat& dst)
{
	cv::Scalar bias = cvScalar(mat.at<float>(0, 0), mat.at<float>(0, 1), mat.at<float>(0, 2));
	cv::Mat T(3, 3, CV_32F, mat.ptr<float>(1));
	cv::Mat srcF;
	src.convertTo(srcF, CV_32F);
	cv::Mat A(src.cols * src.rows, 3, CV_32FC1, srcF.data);
	cv::Mat D = A * T;

	dst = D.reshape(3, src.rows);
	dst = dst + bias;
	dst.convertTo(dst, CV_8U);

	return dst;
}


void SimpleMapper::setup(std::vector<CvScalar>& src, std::vector<CvScalar>& dst)
{
	int N = src.size();
	for (int ch = 0; ch < 3; ch++)
	{
		cv::Mat A(N, 2, CV_32F);
		cv::Mat B(N, 1, CV_32F);
		for (int i = 0; i < src.size(); i++)
		{
			int row = i;
			A.at<float>(row, 0) = 1;
			A.at<float>(row, 1) = src[i].val[ch];
			B.at<float>(row, 0) = dst[i].val[ch];
		}

		cv::Mat X;
		cv::solve(A, B, X, DECOMP_SVD);
		bias.val[ch] = X.at<float>(0, 0);
		scale.val[ch] = X.at<float>(1, 0);
	}
	//print();
}

CvScalar SimpleMapper::doMap(float r, float g, float b)
{
	cv::Scalar input(b, g, r);
	return input * scale + bias;
}

cv::Mat SimpleMapper::mapDense(cv::Mat& src, cv::Mat& dst)
{
	//dst = src * scale + bias;
	int channels = src.channels();
	for (int y = 0; y < src.rows; y++)
	{
		uchar* srcPtr = src.ptr<uchar>(y);
		uchar* dstPtr = dst.ptr<uchar>(y);
		for (int x = 0; x < src.cols; x++)
		{
			for (int ch = 0; ch < channels; ch++)
			{
				int val = srcPtr[x*channels + ch] * scale.val[ch] + bias.val[ch];
				if (val > 255)
				{
					val = 255;
				}
				if (val < 0)
				{
					val = 0;
				}
				dstPtr[x*channels + ch] = val;
			}
		}
	}

	return dst;
}

void SimpleMapper::print()
{
	printf("\nmean:");
	for (int i = 0; i < 3; i++)
	{
		printf("%.3f ", bias.val[i]);
	}
	printf("\n");
	char chs[3] = { 'b', 'g', 'r' };
	for (int i = 0; i < 3; i++)
	{
		printf("%c   : %.3f", chs[i], scale.val[i]);
		printf("\n");
	}
}


void colorThreshold(cv::Mat& color, cv::Mat& mask, float thresh)
{
	int channels = color.channels();
	thresh = channels * thresh * thresh;
	//mask = 0;
	uchar* ptr = mask.ptr<uchar>(0);
	memset(ptr, 0, mask.cols* mask.rows*mask.channels());
	for (int h = 0; h < color.rows; h++)
	{
		uchar* cData = color.ptr<uchar>(h);
		uchar* mData = mask.ptr<uchar>(h);
		for (int w = 0; w < color.cols; w++)
		{
			// channels
			float sum = 0;
			for (int c = 0; c < channels; c++)
			{
				sum += cData[w*channels + c] * cData[w*channels + c];
			}
			if (sum >= thresh)
			{
				mData[w] = 255;
			}
		}
	}
}

ColorMatcher::ColorMatcher()
{
	debug = 0;
	// param
	ybins = 20;
	DarkThresh = 30;
	FgThresh1 = 30;
	FgThresh2 = 45;
	BlurSize = 0;
	RectFgRatio = 0.05f;
}
void ColorMatcher::update1(cv::Mat& src, cv::Mat& dst)
{
	int ysize = src.rows / ybins;
	int xbins = src.cols / ysize;
	int xsize = ysize;
	std::vector<CvScalar> srcList;
	std::vector<CvScalar> dstList;
	//ClockTimer timer;
	// sample from grid
	for (int y = 0; y < ybins; y++)
	{
		for (int x = 0; x < xbins; x++)
		{
			CvRect roi = cvRect(x*xsize, y * ysize, xsize, ysize);
			cv::Mat patchSrc = src(roi);
			cv::Mat patchDst = dst(roi);
			cv::Scalar srcMean = cv::mean(patchSrc);
			cv::Scalar dstMean = cv::mean(patchDst);
			srcList.push_back(srcMean);
			dstList.push_back(dstMean);
		}
	}

	//timer.GetDurationBegin("calc mean");
	lm.setup(srcList, dstList);
	//timer.GetDurationBegin("calc lm");
	//lm.setup(dstList, srcList);
	// create restored image
	res = dst.clone();
	if (0)
	{
		//res = 0;
		uchar* ptr = res.ptr<uchar>(0);
		memset(ptr, 0, res.cols* res.rows*res.channels());
		for (int y = 0; y < ybins; y++)
		{
			for (int x = 0; x < xbins; x++)
			{
				int binIdx = y * xbins + x;
				CvRect roi = cvRect(x*xsize, y * ysize, xsize, ysize);
				cv::Scalar dstColor = lm.mapColor(srcList[binIdx]);
				res(roi) = dstColor;
			}
		}
	}

	//timer.GetDurationBegin("map dense start");
	lm.mapDense(src, res);
	//timer.GetDurationBegin("map dense end");
	// smooth

	// diff
	mask.create(src.rows, src.cols, CV_8U);
	cv::absdiff(res, dst, diff);
	colorThreshold(diff, mask, FgThresh1);
	if (0 && debug)
	{
		diff *= 5;
		// show results
		cv::imshow("res1", res);
		cv::imshow("diff1", diff);
		cv::imshow("mask1", mask);
	}

	//cv::waitKey();
	// refine
	std::vector<CvScalar> srcList2;
	std::vector<CvScalar> dstList2;
	std::vector<int> darkList;
	// sample from grid
	for (int y = 0; y < ybins; y++)
	{
		for (int x = 0; x < xbins; x++)
		{
			int binIdx = y * xbins + x;
			CvRect roi = cvRect(x*xsize, y * ysize, xsize, ysize);
			cv::Mat patchMask = mask(roi);
			cv::Scalar srcMean = cv::mean(patchMask);
			// is dark
			if (srcList[binIdx].val[0] < DarkThresh &&
				srcList[binIdx].val[1] < DarkThresh &&
				srcList[binIdx].val[2] < DarkThresh)
			{
				darkList.push_back(binIdx);
				continue;
			}
			// not foreground
			if (srcMean.val[0] < 0.2)
			{
				srcList2.push_back(srcList[binIdx]);
				dstList2.push_back(dstList[binIdx]);
			}
		}
	}
	//printf("total:%d good:%d dark:%d\n", srcList.size(), srcList2.size(), darkList.size());
	// 
	int darkKeep = MAX(srcList2.size() * 0.2, srcList.size()*0.3 - srcList2.size());
	darkKeep = MIN(darkKeep, darkList.size());
	float darkKeepRatio = float(darkKeep) / darkList.size();
	for (int i = 0; i < darkList.size(); i++)
	{
		float dice = float(rand()) / RAND_MAX;
		int binIdx = darkList[i];
		if (dice <= darkKeepRatio)
		{
			srcList2.push_back(srcList[binIdx]);
			dstList2.push_back(dstList[binIdx]);
		}
	}
	lm.setup(srcList2, dstList2);
	lm.mapDense(src, res);
	// smooth
	if (BlurSize)
	{
		cv::blur(res, res, cv::Size(BlurSize, BlurSize));
	}
	// diff
	cv::absdiff(res, dst, diff);
	colorThreshold(diff, mask, FgThresh2);
	//timer.GetDurationBegin("finish");
	if (debug)
	{
		diff *= 5;
		// show results
		cv::imshow("src", src);
		cv::imshow("dst", dst);
		cv::imshow("res", res);
		cv::imshow("diff", diff);
		cv::imshow("mask", mask);
		cv::waitKey();
	}

}

void ColorMatcher::update(cv::Mat& src, cv::Mat& dst)
{
	int ysize = src.rows / ybins;
	int xbins = src.cols / ysize;
	int xsize = ysize;
	std::vector<CvScalar> srcList;
	std::vector<CvScalar> dstList;
	//ClockTimer timer;
	// sample from grid
	for (int y = 0; y < ybins; y++)
	{
		for (int x = 0; x < xbins; x++)
		{
			CvRect roi = cvRect(x*xsize, y * ysize, xsize, ysize);
			cv::Mat patchSrc = src(roi);
			cv::Mat patchDst = dst(roi);
			cv::Scalar srcMean = cv::mean(patchSrc);
			cv::Scalar dstMean = cv::mean(patchDst);
			srcList.push_back(srcMean);
			dstList.push_back(dstMean);
		}
	}

	//timer.GetDurationBegin("calc mean");
	lm.setup(srcList, dstList);
	//timer.GetDurationBegin("calc lm");

	// estimate matching
	std::vector<float> errorList(srcList.size());
	for (int y = 0; y < ybins; y++)
	{
		for (int x = 0; x < xbins; x++)
		{
			int binIdx = y * xbins + x;
			CvScalar et = lm.mapColor(srcList[binIdx]);
			CvScalar gt = dstList[binIdx];
			float dist = (et.val[0] - gt.val[0]) * (et.val[0] - gt.val[0]) +
				(et.val[1] - gt.val[1]) * (et.val[1] - gt.val[1]) +
				(et.val[2] - gt.val[2]) * (et.val[2] - gt.val[2]);
			errorList[binIdx] = dist;
		}
	}
	std::vector<float> errorTmp(errorList);
	std::sort(errorTmp.begin(), errorTmp.end());
	printf("%f %f\n", errorTmp[0], errorTmp[srcList.size() - 1]);
	int keepIndex = srcList.size() * 0.75f;
	float keepErrorThresh = errorTmp[keepIndex];
#if 0
	// dist histgram
	int distBins = 20;
	std::vector<float> distList(distBins);
	for (int i = 0; i < distList.size(); i++)
	{
		distList[i] = 0;
	}
	float minVal = sqrt(errorTmp[0] / 3);
	float maxVal = sqrt(errorTmp[srcList.size() - 1]/3);
	float valRng = maxVal - minVal;
	float valStep = valRng / distBins;
	for (int i = 0; i < errorList.size(); i++)
	{
		float val = sqrt(errorList[i] / 3);
		int binIdx = (val - minVal) / valStep;
		if (binIdx >= distBins)
		{
			binIdx = distBins - 1;
		}
		if (binIdx < 0)
		{
			binIdx = 0;
		}
		distList[binIdx] += 1;
	}
	DrawHistgram1D(distBins, &distList[0], 1, 20, 400, "dist");
#endif
	// refine
	std::vector<CvScalar> srcList2;
	std::vector<CvScalar> dstList2;
	// sample from grid
	for (int y = 0; y < ybins; y++)
	{
		for (int x = 0; x < xbins; x++)
		{
			int binIdx = y * xbins + x;
			// not foreground
			if (errorList[binIdx] <= keepErrorThresh)
			{
				srcList2.push_back(srcList[binIdx]);
				dstList2.push_back(dstList[binIdx]);
			}
		}
	}
	printf("size:%d\n", srcList2.size());
	//timer.GetDurationBegin("setup2");
	lm.setup(srcList2, dstList2);
	// create restored image
	res = dst.clone();
	lm.mapDense(src, res);
	//timer.GetDurationBegin("mapDense");
	// smooth
	if (BlurSize)
	{
		cv::blur(res, res, cv::Size(BlurSize, BlurSize));
	}
	// diff
	cv::absdiff(res, dst, diff);
	mask.create(src.rows, src.cols, CV_8U);
	colorThreshold(diff, mask, FgThresh2);
	//timer.GetDurationBegin("finish");

	if (debug)
	{
		diff *= 5;
		// show results
		cv::imshow("src", src);
		cv::imshow("dst", dst);
		cv::imshow("res", res);
		cv::imshow("diff", diff);
		cv::imshow("mask", mask);
		cv::Mat mask2 = mask.clone();
		for (int y = 0; y < ybins; y++)
		{
			for (int x = 0; x < xbins; x++)
			{
				int binIdx = y * xbins + x;
				CvRect roi = cvRect(x*xsize, y * ysize, xsize, ysize);
				int type;
				float conf;
				type = getProb(roi, &conf);
				mask2(roi) = conf * 255 * type;
			}
		}
		cv::imshow("mask2", mask2);
		cv::waitKey();
	}
}

int  ColorMatcher::getProb(cv::Rect rect, float* conf)
{
	int ret = 0;
	if (mask.empty())
	{
		if (conf)
		{
			*conf = 0;
		}
		return 0;
	}

	cv::Mat patch = mask(rect);
	cv::Scalar m = cv::mean(patch);
	float ratio = m.val[0] / 255;
	if (ratio > RectFgRatio )
	{
		ret = 1;
	}
	if (conf)
	{
		*conf = ratio;
	}
	return ret;
}

void testMatchColor()
{
	const char* srcDir = "../data/src/";
	const char* dstDir = "../data/dst/";
	const char* oclDir = "../data/dst_o/";
	ColorMatcher matcher;
	for (int i = 1; i < 30; i++)
	{
		char srcPath[260];
		char dstPath[260];
		char oclPath[260];
		sprintf(srcPath, "%s%d.jpg", srcDir, i);
		sprintf(dstPath, "%s%d_warp.jpg", dstDir, i);
		sprintf(oclPath, "%s%d_warp.jpg", oclDir, i);
		cv::Mat src = cv::imread(srcPath);
		cv::Mat dst = cv::imread(oclPath);
		if (dst.empty())
		{
			continue;
		}

		int scale = 4;
		cv::Mat srcResize;
		cv::Mat dstResize;
		cv::resize(src, srcResize, cv::Size(src.cols / scale, src.rows / scale), 0, 0, INTER_NEAREST);
		cv::resize(dst, dstResize, cv::Size(dst.cols / scale, dst.rows / scale), 0, 0, INTER_NEAREST);
		matcher.update(srcResize, dstResize);
	}
	
}