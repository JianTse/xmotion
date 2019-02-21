#include "filter.h"

static bool PairCompare(const std::pair<float, int>& lhs,
	const std::pair<float, int>& rhs) {
	return lhs.first > rhs.first;
}

cv::Scalar  black = cv::Scalar(0, 0, 0);
cv::Scalar  white = cv::Scalar(255, 255, 255);
cv::Scalar  red = cv::Scalar(0, 0, 255);
cv::Scalar  green = cv::Scalar(0, 255, 0);
cv::Scalar  yellow = cv::Scalar(0, 255, 255);

detectFilter::detectFilter()
{
}
detectFilter::~detectFilter()
{
}

void  detectFilter::setRectValue(cv::Mat& img, cv::Rect& rect, cv::Scalar& val)
{
	int  min_y = rect.y;
	int  max_y = rect.y + rect.height;
	int  min_x = rect.x;
	int  max_x = rect.x + rect.width;
	for (int y = min_y; y < max_y; y++)
	{
		uchar* ptr = img.ptr<uchar>(y);
		for (int x = min_x; x < max_x; x++)
		{
			for (int c = 0; c < img.channels(); c++)
			{
				ptr[x*img.channels() + c] = val[c];
			}
		}
	}
}

void  detectFilter::generateMatchRects(cv::Size& imgSize, vector<cv::Rect>& rects)
{
	rects.clear();
	int  grid_y = 10;
	int  grid_x = 10;
	int  grid_w = 80; //imgSize.width / grid_x;
	int  grid_h = 80; //imgSize.height / grid_y;
	int  min_x = grid_w / 2;
	int  max_x = imgSize.width - 1;
	int  min_y = grid_h / 2;
	int  max_y = imgSize.height - 1;
	cv::Rect  rect = cv::Rect(0, 0, grid_w, grid_h);
	for (int y = min_y; y < max_y; y += grid_h)
	{
		for (int x = min_x; x < max_x; x += grid_w)
		{
			rect.x = x - grid_w / 2;
			rect.y = y - grid_h / 2;
			rects.push_back(rect);
		}
	}
}

//计算每相邻两个块之间的相似度
void   detectFilter::getAllMatchInfo(cv::Mat& src, string path)
{
	//首先读这一张图像的所有匹配信息
	int ret = readMatchInfo(path);
	if (ret == 1)
		return;

	cv::Size imgSize = cv::Size(src.cols, src.rows);
	vector<cv::Rect> _rects;
	generateMatchRects(imgSize, _rects);

	int  count = 0;
	_rectInfos.clear();
	float sim = 0.0;
	for (int i = 0; i < _rects.size(); i++)
	{
		rectInfo _info;
		_info.rectId = i;
		_info.rect = _rects[i];
		_info.smallRect = covRect(_rects[i], SCALE);
		getScalarMean(src, _rects[i], _info.mean, _info.dev);
		getRectHist(src, _rects[i], _info.hist);
		_info.state = rectState::unknow;
		_rectInfos.push_back(_info);
	}

	//提取完每个块的直方图，然后计算每两个块之间的相似度
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		for (int j = 0; j < _rectInfos.size(); j++)
		{
			std::pair<float, int> pair;
			if (j < i)   //如果之前已经匹配过了
			{
				sim = _rectInfos[j].sims[i].first;
			}
			else if (j == i)
			{
				sim = 1.0;
			}
			else
			{
				//用颜色直方图计算
				sim = computeSimilarity(_rectInfos[i].hist, _rectInfos[j].hist);

				//计算平方差，没有取均值，考虑颜色
				//sim = getMatchTmpSim(src, _rects[i], src, _rects[j], TM_SQDIFF_NORMED);
				//sim = 1.0 - sim;
			}
			pair.first = sim;
			pair.second = j;
			_rectInfos[i].sims.push_back(pair);
		}

		//对相似度进行排序
		int N = _rectInfos[i].sims.size();
		std::partial_sort(_rectInfos[i].sims.begin(), _rectInfos[i].sims.begin() + N, _rectInfos[i].sims.end(), PairCompare);
	}
	
	//保存这一张图像的匹配信息
	saveMatchInfo(path);

#if  0
	for (int j = 0; j < _rectInfos.size(); j++)
	{
		cv::Mat srcTmp = src.clone();
		for (int i = 0; i < _rectInfos.size(); i++)
		{
			int  rectId = 0;
			for (int n = 0; n < _rectInfos[j].sims.size(); n++)
			{
				if (_rectInfos[j].sims[n].second == i)
				{
					rectId = n;
					break;
				}
			}
			float sim = _rectInfos[j].sims[rectId].first;
			float mean = _rectInfos[i].mean;
			float dev = _rectInfos[i].dev;
			cv::rectangle(srcTmp, _rects[i], cv::Scalar(0, 255, 0));
			char info[255];
			sprintf(info, "%.1f,%.0f,%.0f", sim, mean, dev);
			cv::putText(srcTmp, info, cv::Point(_rects[i].x, _rects[i].y + _rects[i].height / 2), 1, 1, cv::Scalar(0, 0, 255));
		}
		cv::rectangle(srcTmp, _rectInfos[j].rect, cv::Scalar(0, 0, 255));
		cv::imshow("src", srcTmp);
		cv::waitKey(0);
	}
#endif
}

void  detectFilter::saveMatchInfo(string path)
{
	string savePath = find_replace(path, "jpg", "txt");
	FILE* fp = fopen(savePath.c_str(), "w");

	//保存一共有多少个框
	fprintf(fp, "%d\n", _rectInfos.size());
	fflush(fp);
	
	//依次保存每个框的信息
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		//Id,rect,mean,dev
		fprintf(fp, "%d,%d,%d,%d,%d,%f,%f\n", _rectInfos[i].rectId, 
			_rectInfos[i].rect.x, _rectInfos[i].rect.y, _rectInfos[i].rect.width, _rectInfos[i].rect.height,
			_rectInfos[i].mean, _rectInfos[i].dev);
		fflush(fp);		
		//sim
		for (int s = 0; s < _rectInfos[i].sims.size(); s++)
		{
			fprintf(fp, "%f,%d\n", _rectInfos[i].sims[s].first, _rectInfos[i].sims[s].second);
			fflush(fp);
		}
	}
	fclose(fp);
}

int  detectFilter::readMatchInfo(string path)
{
	string savePath = find_replace(path, "jpg", "txt");
	_rectInfos.clear();
	FILE* fp = fopen(savePath.c_str(), "r");
	if (fp == NULL)
		return 0;

	int  rectCount = -1;
	const int status = fscanf(fp, "%d\n", 	&rectCount);

	//依次保存每个框的信息
	int rectId, matchId;
	cv::Rect  rect;
	float mean, dev;
	float sim;
	for (int i = 0; i < rectCount; i++)
	{
		//Id,rect,mean,dev
		const int status = fscanf(fp, "%d,%d,%d,%d,%d,%f,%f\n",
			&rectId, &rect.x, &rect.y, &rect.width, &rect.height, &mean, &dev);
		if (status == EOF) {
			break;
		}

		rectInfo _info;
		_info.rectId = rectId;
		_info.rect = rect;
		_info.smallRect = covRect(rect, SCALE);
		_info.mean = mean;
		_info.dev = dev;
		_info.state = rectState::unknow;		

		//sim
		for (int s = 0; s < rectCount; s++)
		{
			std::pair<float, int> pair;
			const int status = fscanf(fp, "%f,%d\n",&sim, &matchId);
			if (status == EOF) {
				break;
			}
			pair.first = sim;
			pair.second = matchId;
			_info.sims.push_back(pair);
		}

		_rectInfos.push_back(_info);
	}
	fclose(fp);

	if (_rectInfos.size() == rectCount)
		return 1;
	return 0;
}

void  detectFilter::getRectHist(cv::Mat& img, cv::Rect& rect, vector<float>& hist)
{
	hist.clear();
	hist.resize(HIST_LEN);
	memset(&hist[0], 0, HIST_LEN);
	int  min_y = rect.y;
	int  max_y = rect.y + rect.height;
	int  min_x = rect.x;
	int  max_x = rect.x + rect.width;
	for (int y = min_y; y < max_y; y++)
	{
		uchar* ptr = img.ptr<uchar>(y);
		for (int x = min_x; x < max_x; x++)
		{
			int b = ptr[x * 3] / BIN_B;
			int g = ptr[x * 3 + 1] / BIN_G;
			int r = ptr[x * 3 + 2] / BIN_R;
			int bin_id = r * BIN_G * BIN_B + g * BIN_B + b;
			hist[bin_id]++;
		}
	}
	normalize(hist);
}

void detectFilter::normalize(vector<float>& hist)
{
	double sum = 0;
	for (unsigned int i = 0; i < hist.size(); ++i)
		sum += hist[i];
	for (unsigned int i = 0; i < hist.size(); ++i)
		hist[i] /= sum;
}

float detectFilter::computeSimilarity(vector<float>& hist1, vector<float>& hist2)
{
	float conf = 0;
	for (unsigned int i = 0; i < hist1.size(); ++i) {
		conf += sqrt(hist1[i] * hist2[i]);
	}
	return conf;
}

void detectFilter::getScalarMean(cv::Mat& img, cv::Rect rect, float& mean, float& dev)
{
	cv::Mat imgTmp = img(rect);

	float sum_mean = 0;
	float sum_dev = 0;
	Mat mat_mean, mat_stddev;
	std::vector<cv::Mat> channels_r;
	cv::split(imgTmp, channels_r);
	for (int i = 0; i < channels_r.size(); i++)
	{
		meanStdDev(channels_r[i], mat_mean, mat_stddev);

		float m = mat_mean.at<double>(0, 0);
		float s = mat_stddev.at<double>(0, 0);
		sum_mean += m;
		sum_dev += s;
	}
	dev = sum_dev / channels_r.size();
	mean = sum_mean / channels_r.size();
}

//在背景中找最
int  detectFilter::findSimilyRectId(int curId)
{
	for (int i = 1; i < _rectInfos[curId].sims.size(); i++)
	{
		int rectId = _rectInfos[curId].sims[i].second;
		if (_rectInfos[rectId].state == rectState::back)
		{
			return  rectId;
		}
	}
	return -1;
}

//用模板匹配初步提取可靠前景背景
void detectFilter::firstMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask)
{
	
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		//如果这个框是背景就不用搞了
		if (_rectInfos[i].state == rectState::back)
			continue;

		if (_rectInfos[i].dev > 16)  //如果方差足够大，则用模板匹配
		{
			//计算归一化的相关性系数，去均值，主要考虑纹理
			float sim = getMatchTmpSim(src, _rectInfos[i].smallRect, dst, _rectInfos[i].smallRect, TM_CCOEFF_NORMED);
			if (sim > 0.65)  //一定没有被遮挡
			{
				_rectInfos[i].state = rectState::back;
				cv::Scalar val = cv::Scalar(0, 0, 0, 0);
				setRectValue(mask, _rectInfos[i].smallRect, val);
			}
		}
	}
}

void  detectFilter::secondMatch(cv::Mat& src, cv::Mat& dst, cv::Mat& mask)
{
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		//如果这个框是背景就不用搞了
		if (_rectInfos[i].state == rectState::back)
			continue;

		//同时方差太大时，不能用均值判断
		//if (_rectInfos[i].dev >48)
		//	continue;

		//如果当前块不确定
		//if (_rectInfos[i].state == rectState::unknow)
		{
			int id = findSimilyRectId(i);  //找到与当前块颜色最相似、并且没有被遮挡的块

			//原始图像中均值的差异
			float  srcDiff = fabs(_rectInfos[i].mean - _rectInfos[id].mean);

			//warp图像中的均值的差异
			float mean1, dev1, mean2, dev2;
			getScalarMean(dst, _rectInfos[i].smallRect, mean1, dev1);
			getScalarMean(dst, _rectInfos[id].smallRect, mean2, dev2);
			float  dstDiff = fabs(mean1 - mean2);			

			float  thresh = std::max(32.0f, srcDiff / 2.5f);
			if (dstDiff < thresh)
			{
				_rectInfos[i].state = rectState::back;
				//把这个块赋值为0
				cv::Scalar val = cv::Scalar(0, 0, 0, 0);
				setRectValue(mask, _rectInfos[i].smallRect, val);
			}
			else
			{
				//LOGI("src diff: %f, dstDiff: %f\n", srcDiff, dstDiff);
			}
		}
	}

	//LOGI(" over  \n\n");
}

void   detectFilter::match(cv::Mat& src, cv::Mat& dst, cv::Mat& mask)
{
	if (src.empty() || dst.empty())
		return;
	if (src.channels() != dst.channels())
		return;	
	
	cv::Scalar  color = yellow;

	//初步提取前景背景
	firstMatch(src, dst, mask);
	//secondMatch(src, dst);

	cv::Mat  dstTmp = dst.clone();
	//用cnn提取的方法
	char info[255];
	vector<float> srcFeats, dstFeats;
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		cv::Point pt = getRectCenter(_rectInfos[i].smallRect);

		color = yellow;
		switch (_rectInfos[i].state)
		{
		case rectState::back:
			color = green;
			break;
		case rectState::fore:
			color = red;
			cv::circle(dst, pt, 1, color, 1);
			break;
		default:
			color = yellow;
			break;
		}
		
		cv::rectangle(dstTmp, _rectInfos[i].smallRect, color);
		sprintf(info, "%.0f,%.0f", _rectInfos[i].mean, _rectInfos[i].dev);
		//cv::putText(dstTmp, info, cv::Point(_rectInfos[i].smallRect.x, _rectInfos[i].smallRect.y + _rectInfos[i].smallRect.height / 2), 1, 1, color);
	}

	//cv::imshow("src", srcTmp);
	cv::imshow("dst", dstTmp);
	//cv::waitKey(0);
}

void  detectFilter::run(string bkFn, cv::Mat& src, cv::Mat& smallSrc, cv::Mat& smallDst, ColorMatcher& _color)
{
	if (smallDst.empty() || src.empty())
		return;
	if (smallDst.cols != smallSrc.cols || smallSrc.rows != smallDst.rows)
		return;

	//
	getAllMatchInfo(src, bkFn);

	//给每个框状态赋值
	for (int i = 0; i < _rectInfos.size(); i++)
	{
		int state = _color.getProb(_rectInfos[i].smallRect);
		if (state == 0)
		{
			_rectInfos[i].state = rectState::back;
			cv::Scalar val = cv::Scalar(0, 0, 0, 0);
			setRectValue(_color.mask, _rectInfos[i].smallRect, val);
		}
		else
		{
			_rectInfos[i].state = rectState::fore;
		}
	}

	//match(smallSrc, smallDst, _color.mask);
	firstMatch(smallSrc, smallDst, _color.mask);

	secondMatch(smallSrc, smallDst, _color.mask);
}
