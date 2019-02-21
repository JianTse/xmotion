/*                      OpenCV headers
Purpose : use opencv with just one line of code
Author  : tianzekang@ysten.com
Modifies:
	tianzekang   2012-03-09   created
	tianzekang   2012-05-08   add static funcitons
*/

#ifndef __OPEN_CV__
#define __OPEN_CV__

#include <tzk/keyboard.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cvaux.h>
#include <opencv/cxmisc.h>

#define HAVE_OPENCV 1

#ifndef CVLIBVER
#define CVLIBVER "249"
#endif

#ifdef _DEBUG 
#define CVLIBEXT "d.lib"
#else
#define CVLIBEXT ".lib"
#endif

#define CVLIBNAME(name,ver,ext) "opencv_" name ver ext

#pragma comment(lib,CVLIBNAME("core",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("highgui",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("video",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("ml",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("legacy",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("imgproc",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("features2d",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("flann",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("objdetect",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("calib3d",CVLIBVER,CVLIBEXT))
#pragma comment(lib,CVLIBNAME("contrib",CVLIBVER,CVLIBEXT))


//using namespace cv;

#define IMAGE_AT(type,image,x,y)   *(type*)(image->imageData+image->widthStep*(y)+image->nChannels*(x)*sizeof(type))

static void cvWaitForPause(int ms)
{
	int key = cvWaitKey(ms);
	if (key == KEY_SPACE)
	{
		cvWaitKey(0);
	}
}

/** 从直方图计算图像的均值方差
 *
 */
static void CalcHistMeanDev(int bins,float hist[],float *mean,float* dev)
{
	float sum = 0.0;
	float square = 0.0;
	float count = 0.0;
	float m = 0;
	for (int i = 0;i < bins;i++)
	{
		sum += hist[i]*i;
		square += hist[i]*i*i;
		count += hist[i];
	}

	m = sum/count;
	*mean = m;
	*dev = sqrt(square/count - m*m);
}

/** 计算数据的均值方差
*
*/
static void CalcMeanDev(int n,float hist[],float *mean,float* dev)
{
	float sum = 0.0;
	float square = 0.0;
	float m = 0;
	for (int i = 0;i < n;i++)
	{
		sum += hist[i];
		square += hist[i]*hist[i];
	}

	m = sum/n;
	*mean = m;
	*dev = sqrt(square/n - m*m);
}

/************************************************************************/
/*                           DRAWING STUFF                              */
/************************************************************************/

static CvScalar LabelToColorRGB(int label,uchar &r,uchar &g,uchar& b)
{
	int colorValue = label*4571251;
	b = colorValue & 0xff;
	colorValue = colorValue >> 8;
	g = colorValue & 0xff;
	colorValue = colorValue >> 8;
	r = colorValue & 0xff;

	return cvScalar(b,g,r);
}

static CvScalar LabelToColor(int label,uchar &r,uchar &g,uchar& b)
{
	int colorValue = label*4571251;
	b = colorValue & 0xff;
	colorValue = colorValue >> 8;
	g = colorValue & 0xff;
	colorValue = colorValue >> 8;
	r = colorValue & 0xff;

	return cvScalar(b,g,r);
}

static CvScalar cvLabelToColor(int label)
{
	uchar r,g,b;
	int colorValue = label*4571251;
	b = colorValue & 0xff;
	colorValue = colorValue >> 8;
	g = colorValue & 0xff;
	colorValue = colorValue >> 8;
	r = colorValue & 0xff;

	return cvScalar(b,g,r);
}

static CvScalar LabelToColorInvert(int label,uchar &r,uchar &g,uchar& b)
{
	LabelToColorRGB(label,r,g,b);
	r = 255 - r;
	g = 255 - g;
	b = 255 - b;

	return cvScalar(b,g,r);
}

static void cvDrawText(IplImage *image,CvPoint point,const char* str,CvScalar color = cvScalar(255,255,255) ,float size = 1.0)
{
	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX ,size,size,0,cvRound(size*2),8);
	cvPutText(image,str,point,&font,color);
}

static void cvDrawBox(IplImage* image,CvRect rect,CvScalar color,int thickness = CV_FILLED)
{
	cvDrawRect(image,cvPoint(rect.x,rect.y),cvPoint(rect.x+rect.width,rect.y+rect.height),color,thickness);	
}

static void cvDrawPoints(IplImage* image,CvPoint* points,int pointSize,CvScalar lineColor,CvScalar ptColor)
{
	// draw the bitch
	cvPolyLine(image,&points,&pointSize,1,0,lineColor,2);

	for (int i = 0; i < pointSize; i++)
	{
		cvCircle(image,points[i],4,ptColor,CV_FILLED);
	}
}

/**
* \bins    直方图的bin数
* \hist    直方图数据
* \drawBinStep 绘制的bin数
* \drawBinWidth 每个bin绘制的宽度
* \drawHeight   直方图图像的高度
*/
static void DrawHistgram1D(int bins,float hist[],int drawBinStep,int drawBinWidth,int drawHeight,char* windowName)
{
	float minValue,maxValue;
	float totoalValue = 0.0;
	int maxPos = 0;
	float mean,dev;

	// 待显示的直方图bin数
	int   mergeHistBins = (bins + drawBinStep - 1)/drawBinStep;
	float* mergeHist = new float[mergeHistBins];

	IplImage* histImage = cvCreateImage(cvSize(drawBinWidth*mergeHistBins,drawHeight),8,3);

	cvZero(histImage);
	memset(mergeHist,0,sizeof(float)*mergeHistBins);

	// 合并直方图
	for (int i = 0;i < bins;i++)
	{
		mergeHist[i/drawBinStep] += hist[i];
		totoalValue += hist[i];
	}

	if (totoalValue < 0.9)
	{
		return;
	}

	// 找到最大最小值
	minValue = maxValue = mergeHist[0];
	for(int i = 1;i < mergeHistBins;i++)
	{
		if (mergeHist[i] < minValue)
		{
			minValue = mergeHist[i];
		}
		else if (mergeHist[i] > maxValue)
		{
			maxValue = mergeHist[i];
			maxPos = i;
		}
	}

	// 计算均值和方差
	CalcHistMeanDev(bins,hist,&mean,&dev);

	// draw the bitch
	if (1 == drawBinWidth)
	{
		for (int i = 0;i < mergeHistBins;i++)
		{
			int height = int(mergeHist[i]*drawHeight/maxValue);
			int density = mergeHist[i]*255/maxValue;

			cvLine(histImage,cvPoint(i*drawBinWidth,drawHeight - height),cvPoint(i*drawBinWidth,drawHeight - 1),
				cvScalar(density,density,255-density),drawBinWidth );
		}
	}
	else
	{
		for (int i = 0;i < mergeHistBins;i++)
		{
			int height = mergeHist[i]*drawHeight/maxValue;
			int density = mergeHist[i]*255/maxValue;

			cvRectangle(histImage,cvPoint(i*drawBinWidth,drawHeight - height),cvPoint((i+1)*drawBinWidth,drawHeight - 1),
				cvScalar(density,density,255),CV_FILLED );
		}
	}

	// 绘制均值和方差
	int meanBin = cvRound(mean);
	int devBin = cvRound(dev);
	int x1 = meanBin*drawBinWidth;
	int x2 = (meanBin+devBin)*drawBinWidth;
	int x3 = (meanBin - devBin)*drawBinWidth;

	cvLine(histImage,cvPoint(x1,0),cvPoint(x1,histImage->height),
		cvScalar(0,255,0),1 );

	cvLine(histImage,cvPoint(x2,0),cvPoint(x2,histImage->height),
		cvScalar(255,0,0),1 );

	cvLine(histImage,cvPoint(x3,0),cvPoint(x3,histImage->height),
		cvScalar(0,0,255),1 );

	// cvDrawText
	char text[200];
	sprintf(text,"[%3d]:%5.0f",maxPos,maxValue);
	cvDrawText(histImage,cvPoint(10,30),text,cvScalar(0,255,0),0.8);

	sprintf(text,"[%6.2f %6.2f]",mean,dev);
	cvDrawText(histImage,cvPoint(10,60),text,cvScalar(0,255,0),0.8);

	cvShowImage(windowName,histImage);
	delete [] mergeHist;
	cvReleaseImage(&histImage);
}


#endif /*__OPEN_CV__*/