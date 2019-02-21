#include "detect.h"
#ifdef WIN32
#include<io.h>
#else
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

int trave_dir(string path, int depth, vector<string>& fns)
{
	DIR *d; //����һ�����
	struct dirent *file; //readdir�����ķ���ֵ�ʹ��������ṹ����
	struct stat sb;

	if (!(d = opendir(path.c_str())))
	{
		LOGI("error opendir %s!!!/n", path.c_str());
		return -1;
	}
	while ((file = readdir(d)) != NULL)
	{
		//�ѵ�ǰĿ¼.����һ��Ŀ¼..�������ļ���ȥ����������ѭ������Ŀ¼
		if (strncmp(file->d_name, ".", 1) == 0)
			continue;
		//strcpy(filename[len++], file->d_name); //������������ļ���
		fns.push_back(file->d_name);
		//�жϸ��ļ��Ƿ���Ŀ¼�����Ƿ������������㣬�����Ҷ���ֻ����������Ŀ¼��̫��Ͳ����ˣ�ʡ���ѳ�̫���ļ�
		if (stat(file->d_name, &sb) >= 0 && S_ISDIR(sb.st_mode) && depth <= 3)
		{
			trave_dir(file->d_name, depth + 1, fns);
		}
	}
	closedir(d);
	return 0;
}
#endif

#define random(x) (rand()%x)
int RandNumber(int begin = 0, int end = 1) {
	return (rand() % (end - begin + 1)) + begin;
}

detectBase::detectBase()
{
	_inited = 0;
	_imageSize = cv::Size(bigWidth,bigHeight);
	_frameCounter = 0;
	_respondCount = 0;
	_switchImgStamp = 0;
	_changeSleepThresh = 15;
	generateCelibRects(_imageSize, _celibRects);
	srand((int)time(0));
	_totalFiles.clear();
}
detectBase::~detectBase()
{
}

int  detectBase::init(string  imgDir)
{
	//��������ͼƬ
	_totalFiles.clear();
	getFiles(imgDir, _totalFiles);
	int ret = _celib.init(imgDir);  //��ʼ��
	if (ret == 0)
		return 0;	

	//��ȡ�˶���Χ
	_celib.getMotionRange(_range);
	
	//����������Ϸ��
	_gameRects.clear();
	_gameRectsResponds.clear();
	for (int i = 0; i < 5; i++)
	{
		cv::Rect rect = generateRandRect(_imageSize, _range, _gameRects, 0);
		_gameRects.push_back(rect);
		_gameRectsResponds.push_back(0);
	}

	_inited = 1;
	return 1;
}

int  detectBase::destory()
{
	_inited = 0;
	_frameCounter = 0;
	_respondCount = 0;
	_celib.destroy();
	_motion.destroy();
	_gameRects.clear();
	_gameRectsResponds.clear();
	return 1;
}

void detectBase::getFiles(string path, vector<string>& files)
{
	files.clear();
#ifdef WIN32
	//�ļ����  
	long   hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(path).append("\\").append(fileinfo.name), files);
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
#else
	trave_dir(path, 2, files);
	for (int i = 0; i < files.size(); i++)
	{
		files[i] = path + files[i];
	}
#endif

	vector<string> fileTmps;
	for (int i = 0; i < files.size(); i++)
	{
		cv::Mat img = cv::imread(files[i].c_str());
		if (img.empty())
			continue;
		if (img.cols != bigWidth || img.rows != bigHeight)
			continue;
		fileTmps.push_back(files[i]);
		LOGI("img path: %s", files[i].c_str());
	}
	files.reserve(fileTmps.size());
	files.assign(fileTmps.begin(), fileTmps.end());

	LOGI("img num: %d", files.size());	
	if (files.size() > 0)
	{
		int idx = RandNumber(0, files.size()-1);  //��ͼ��������ѡ��һ��ͼƬ
		_imgPath = files[idx];
		_src = cv::imread(_imgPath.c_str());
	}	
}

void  detectBase::generateCelibRects(cv::Size& imgSize, vector<cv::Rect>& celibRects)
{
	celibRects.clear();

	int  grid_y = 10;
	int  grid_x = 10;
	int  grid_w = imgSize.width / grid_x;
	int  grid_h = imgSize.height / grid_y;
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
			if (isValidRect(imgSize, rect))
			{
				celibRects.push_back(rect);
			}
		}
	}
	celibRects.push_back(cv::Rect(0, 0, 0, 0));
}

int detectBase::rectInRange(cv::Rect& rect, vector<cv::Point>& range)
{
	cv::Point2f pt;
	double ret = 0;

	//�ж�rect��ÿ�����Ƿ��ڶ�����ڲ�
	pt = cv::Point2f(rect.x, rect.y);
	ret = pointPolygonTest(range, pt, true);
	if (ret < 0)
		return 0;
	pt = cv::Point2f(rect.x+rect.width, rect.y);
	ret = pointPolygonTest(range, pt, true);
	if (ret < 0)
		return 0;
	pt = cv::Point2f(rect.x+rect.width, rect.y+rect.height);
	ret = pointPolygonTest(range, pt, true);
	if (ret < 0)
		return 0;
	pt = cv::Point2f(rect.x, rect.y+rect.height);
	ret = pointPolygonTest(range, pt, true);
	if (ret < 0)
		return 0;

	return 1;
}
cv::Rect  detectBase::randRect(cv::Size& imgSize)
{
	int  grid_y = 10;
	int  grid_x = 10;
	int  grid_w = imgSize.width / grid_x;
	int  grid_h = imgSize.width / grid_x;

	int  min_x = grid_w;
	int  max_x = imgSize.width - grid_w;
	int  min_y = grid_h;
	int  max_y = imgSize.height - grid_h;

	int cx = RandNumber(min_x, max_x);
	int cy = RandNumber(min_y, max_y);
	int x = cx - grid_w / 2;
	int y = cy - grid_h / 2;
	cv::Rect  rect = cv::Rect(x, y, grid_w, grid_h);
	return rect;
}

int  detectBase::isOverlapRect(vector<cv::Rect>& rects, cv::Rect& curRect)
{
	for (int i = 0; i < rects.size(); i++)
	{
		float ov = getRectOverlap(rects[i], curRect);
		if (ov > 0.1 || ov < 0)
		{
			return 0;
		}
	}
	return 1;
}

cv::Rect  detectBase::generateRandRect(cv::Size& imgSize, vector<cv::Point>& range, vector<cv::Rect>& rects, int type)
{
	cv::Rect retRect = cv::Rect(0, 0, 0, 0);
	float  maxSim = 0.0f;
	int  totalCount = 0;
	int  simCount = 0;
	while (true)
	{
		cv::Rect  curRect = randRect(imgSize);   //����һ����
		int ov = isOverlapRect(rects, curRect);  //�������������������ص���
		int inPoly = rectInRange(curRect, range);  //�ж�������Ƿ��ڶ�����ڲ�	
		totalCount++;

		//����ǵ�һ�Σ���ֻ��Ҫ�жϲ����������ص���ͬʱ��Ҫ��ָ����Χ��
		if (type == 0)
		{
			if (ov == 1 && inPoly == 1)
			{
				totalCount = 1000;
				retRect = curRect;
				break;
			}				
		}
		else
		{
			//�������û���˶�
			cv::Rect smallRect = covRect(curRect, SCALE);
			float  motionRatio = getMaskFrRatio(_mask, smallRect);

			//ֻ�������Ȳ����������ص���ͬʱ��Ҫ��ָ����Χ�ڣ����Ҹÿ�û�б��ڵ�������Ч
			if (ov == 1 && inPoly == 1 && motionRatio < 0.01f)
			{
				//��5�Σ�ѡ�����ƶȣ��ͱ��������Ŀ�
				simCount++;
				float sim = getMatchTmpSim(_src, curRect, _camSmall, smallRect, TM_CCOEFF_NORMED);
				if (sim > maxSim)
				{
					maxSim = sim;
					retRect = curRect;
				}
			}						
		}		
		//��������˺ܶ�Σ���ֱ��������
		if (simCount > 5 || totalCount > 100)
		{
			LOGI("break: %d, %d, %f", totalCount, simCount, maxSim);
			break;
		}
	}
	return retRect;
}

int  detectBase::correctCamera(cv::Mat& nv21)
{
	if (_inited == 1)
		return 1;

	//��ȡ������״̬
	int celibState = _celib.getState();

	//��ȡ�������id
	int  celibId = _celib.getCelibRectId();

	//ִ�н���
	if (celibId >= _celibRects.size() - 1)   
		celibId = _celibRects.size() - 1;
	_celib.run(nv21, _celibRects[celibId]);

	//��������ɹ�
	if (celibState == camera_state::celib_over)
	{
		//��ȡ�˶���Χ
		_celib.getMotionRange(_range);

		//����������Ϸ��
		_gameRects.clear();
		_gameRectsResponds.clear();
		for (int i = 0; i < 5; i++)
		{
			cv::Rect rect = generateRandRect(_imageSize, _range, _gameRects, 0);
			_gameRects.push_back(rect);
			_gameRectsResponds.push_back(0);
		}
		_inited = 1;
	}

	return 1;
}

int  detectBase::getState()
{
	return _celib.getState();
}
string  detectBase::getImgPath()
{
	return _imgPath;
}

int  detectBase::getRectInfo(cv::Rect& celibRect, vector<cv::Rect>& gameRects, vector<int>& gameResponds)
{
	//��ȡ�ÿ�
	int  celibId = _celib.getCelibRectId();
	celibRect = _celibRects[celibId];

	//�����Ϸ�鼰��Ӧ����
	gameRects.reserve(_gameRects.size());
	gameRects.assign(_gameRects.begin(), _gameRects.end());
	gameResponds.reserve(_gameRectsResponds.size());
	gameResponds.assign(_gameRectsResponds.begin(), _gameRectsResponds.end());

	return 1;
}

int  detectBase::run(cv::Mat& nv21)
{
	//�ж������Ƿ���ȷ
	if (nv21.empty())
		return 0;
	if (nv21.cols != bigWidth || nv21.rows != bigHeight * 3 / 2)
		return 0;
	if (_totalFiles.size() < 1 || _src.empty())
	{
		LOGI("back ground not exist");
		return 0;
	}		

	//�������
	correctCamera(nv21);

	//����������ʧ��
	if (_inited == 0)
		return 0;

	_frameCounter++;	
	if (_frameCounter < 50)
		return 0;

	//������celib����ͼ��
	//double  t1 = getTimeStamp();
	_celib.getProcessImg(_src, nv21, _srcSmall, _camSmall);
	//double  t2 = getTimeStamp();

	//ֱ��֡��
	int ret = _motion.run(_camSmall);
	//double  t3 = getTimeStamp();
	//LOGI("run time: %.2f,%.2f", t2 - t1, t3 - t2);
	_mask = _motion.getMotionMask();
	//cv::Mat dstMask = _mask;
	if (ret == 0)
		return 0;

	//��������ϴ��л�ͼ��ʱ����̫�̣��򲻴���
	if (_frameCounter - _switchImgStamp < _changeSleepThresh)
		return 0;

	//LOGI("proc : %d", _frameCounter);
	//�����ж�ÿ����Ϸ���Ƿ�����Ӧ	
	for (int i = 0; i < _gameRects.size(); i++)
	{
		cv::Rect smallRect = covRect(_gameRects[i], SCALE);
		float  motionRatio = getMaskFrRatio(_mask, smallRect);
		if (motionRatio > 0.075f)  //����������ǰ���ˣ����������Ӧ
		{
			cv::Rect rect = generateRandRect(_imageSize, _range, _gameRects, 1);
			_gameRects[i] = rect;
			_gameRectsResponds[i]++;
			_respondCount++;
			_switchImgStamp = _frameCounter;
			_changeSleepThresh = 15;
		}
	}

	//���ݽ���л�ͼ��
	if (_respondCount >= 10)
	{
		_respondCount = 0;
		int idx = RandNumber(0, _totalFiles.size()-1);  //��ͼ��������ѡ��һ��ͼƬ
		_imgPath = _totalFiles[idx];
		LOGI("change pic: %s", _imgPath.c_str());

		//�����Ӧ�ˣ��������һ��ͼƬ���������滭
		_src = cv::imread(_imgPath.c_str());
		_switchImgStamp = _frameCounter;
		_changeSleepThresh = 20;
	}

#if  0
	//��ʾ����Ա�ͼ
	cv::Mat  colorBgrMask;
	cv::Rect  roi = cv::Rect(0, 0, _camSmall.cols, _camSmall.rows);
	cv::Mat  showImg = cv::Mat::zeros(cv::Size(_camSmall.cols * 2, _camSmall.rows * 2), CV_8UC3);
	//���Ͻǣ�ԭʼͼ
	cv::resize(_src, _srcSmall, cv::Size(_camSmall.cols, _camSmall.rows));
	cv::Rect(0, 0, _camSmall.cols, _camSmall.rows);
	_srcSmall.copyTo(showImg(roi));
	//���Ͻǣ�camͼ
	roi = cv::Rect(_camSmall.cols, 0, _camSmall.cols, _camSmall.rows);
	_camSmall.copyTo(showImg(roi));
	//���½ǣ�_color���ͼ
	roi = cv::Rect(0, _camSmall.rows, _camSmall.cols, _camSmall.rows);
	//cv::cvtColor(_motion._mergeMask, colorBgrMask, CV_GRAY2BGR);
	//colorBgrMask.copyTo(showImg(roi));
	//���½ǣ�filter���ͼ
	roi = cv::Rect(_camSmall.cols, _camSmall.rows, _camSmall.cols, _camSmall.rows);
	cv::cvtColor(_mask, colorBgrMask, CV_GRAY2BGR);
	colorBgrMask.copyTo(showImg(roi));
#endif

#ifdef WIN32
	int idx = RandNumber(0, _totalFiles.size()-1);  //��ͼ��������ѡ��һ��ͼƬ
	LOGI("imgPath idx: %d\n", idx);
	_imgPath = _totalFiles[idx];
	_src = cv::imread(_imgPath.c_str());

	//cv::imshow("img", showImg);
	//cv::waitKey(0);
#else
	//д����ļ�
	//cv::imwrite("./sdcard/DCIM/xj/test.jpg", showImg);
#endif
	return 1;
}

void   detectBase::detectTest()
{
	std::string  imgDir = "D:/xmotion/data/src/";
	int ret = init(imgDir);  //��ʼ��
	if (ret == 0)
		return;

	while (1)
	{
		LOGI("cur imgPath: %s\n", _imgPath.c_str());

		string dstPath = find_replace(_imgPath, "src", "dst_m1");
		cv::Mat dst = imread(dstPath.c_str());
		if (dst.empty()) continue;

		//��cam����תΪnv21��ʽ
		int bigNv21Size = dst.cols*dst.rows + (dst.cols * dst.rows) / 2;
		uchar *  bigNv21 = new uchar[bigNv21Size];
		RgbToNv12_ptr(bigNv21, dst.data, dst.cols, dst.rows);
		cv::Mat  nv21Big(dst.rows * 3 / 2, dst.cols, CV_8UC1, bigNv21);

		run(nv21Big);

		delete[] bigNv21;
	}
}


