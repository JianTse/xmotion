#ifndef _DETECT_FILTER__H__
#define _DETECT_FILTER__H__
#include "gameCommon.h"
#include "colormap.hpp"

struct rectInfo
{
	int rectId;   //��ǰrect���
	cv::Rect  rect;
	cv::Rect  smallRect;
	vector<float> hist;
	float  mean;
	float  dev;
	int  state;
	std::vector<std::pair<float, int> > sims;   //��ǰ��������������ƶȣ�����������
};

class detectFilter {
public:
	detectFilter();
	~detectFilter();

	//�ⲿ�ӿں���������ԭʼͼ���·��������ͷ�ɼ�warp��ͼ���������rect��״̬
	void  run(string bkFn, cv::Mat& src, cv::Mat& smallSrc, cv::Mat& smallDst, ColorMatcher& _color);

	vector<rectInfo>  _rectInfos;
private:	
	
	//���ɿ�
	void  generateMatchRects(cv::Size& imgSize, vector<cv::Rect>& rects);

	//ֱ��ͼ��������������ƶ�
	void normalize(vector<float>& hist);
	float computeSimilarity(vector<float>& hist1, vector<float>& hist2);
	void getRectHist(cv::Mat& img, cv::Rect& rect, vector<float>& hist);	

	//ģ��ƥ���������������ƶ�
	//float  getMatchTmpSim(cv::Mat& img1, cv::Rect& rect1, cv::Mat& img2, cv::Rect& rect2, int method);

	//��ȡ����ͼ���ÿ�����������Ϣ
	void   getAllMatchInfo(cv::Mat& src, string path);

	void  saveMatchInfo(string path);
	int  readMatchInfo(string path);

	//��ȡͼ���ľ�ֵ������
	void getScalarMean(cv::Mat& img, cv::Rect rect, float& mean, float& dev);

	//Ѱ��ԭʼͼ���п������Ƶ����������һ�Ҫ�Ǳ���
	int  findSimilyRectId(int curId);

	void firstMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void secondMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void match(cv::Mat& src, cv::Mat& dst, cv::Mat& mask);

	void  setRectValue(cv::Mat& img, cv::Rect& rect, cv::Scalar& val);
};

#endif