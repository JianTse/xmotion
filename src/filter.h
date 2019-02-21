#ifndef _DETECT_FILTER__H__
#define _DETECT_FILTER__H__
#include "gameCommon.h"
#include "colormap.hpp"

struct rectInfo
{
	int rectId;   //当前rect序号
	cv::Rect  rect;
	cv::Rect  smallRect;
	vector<float> hist;
	float  mean;
	float  dev;
	int  state;
	std::vector<std::pair<float, int> > sims;   //当前框与其它框的相似度，其它框的序号
};

class detectFilter {
public:
	detectFilter();
	~detectFilter();

	//外部接口函数，输入原始图像的路径，摄像头采集warp后图像，输出所有rect的状态
	void  run(string bkFn, cv::Mat& src, cv::Mat& smallSrc, cv::Mat& smallDst, ColorMatcher& _color);

	vector<rectInfo>  _rectInfos;
private:	
	
	//生成块
	void  generateMatchRects(cv::Size& imgSize, vector<cv::Rect>& rects);

	//直方图计算两个块的相似度
	void normalize(vector<float>& hist);
	float computeSimilarity(vector<float>& hist1, vector<float>& hist2);
	void getRectHist(cv::Mat& img, cv::Rect& rect, vector<float>& hist);	

	//模板匹配计算两个块的相似度
	//float  getMatchTmpSim(cv::Mat& img1, cv::Rect& rect1, cv::Mat& img2, cv::Rect& rect2, int method);

	//获取背景图像的每个块的所有信息
	void   getAllMatchInfo(cv::Mat& src, string path);

	void  saveMatchInfo(string path);
	int  readMatchInfo(string path);

	//提取图像块的均值、方差
	void getScalarMean(cv::Mat& img, cv::Rect rect, float& mean, float& dev);

	//寻找原始图像中块最相似的索引，并且还要是背景
	int  findSimilyRectId(int curId);

	void firstMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void secondMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void match(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void  setRectValue(cv::Mat& img, cv::Rect& rect, cv::Scalar& val);
};

#endif