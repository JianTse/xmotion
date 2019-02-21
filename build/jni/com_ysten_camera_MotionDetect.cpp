#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "com_ysten_camera_MotionDetect.h"
#include "../../src/detect.h"

static detectBase  _detector;

JNIEXPORT jint JNICALL Java_com_ysten_camera_MotionDetect_init
(JNIEnv *env, jobject obj,jstring jdir)
{
        const char* imgDir = (env)->GetStringUTFChars(jdir, 0);
        int ret = _detector.init(imgDir);
        return ret;
}

JNIEXPORT jint JNICALL Java_com_ysten_camera_MotionDetect_run
  (JNIEnv *env, jobject obj, jbyteArray data, jint width, jint height)
  {
        //获取图像数据指针
	unsigned char *srcImage = (unsigned char *)env->GetByteArrayElements(data,NULL);
        
        //初始化图像
        cv::Mat  img(height*3/2, width, CV_8UC1, srcImage);

        //计算响应
        _detector.run(img);

	(env)->ReleaseByteArrayElements( data, (jbyte*)srcImage, 0);
	return 1;
}

JNIEXPORT jint JNICALL Java_com_ysten_camera_MotionDetect_getState
  (JNIEnv *env, jobject obj)
  {
        int  state = _detector.getState();
        return state;
  }
  
JNIEXPORT jstring JNICALL Java_com_ysten_camera_MotionDetect_getImgPath
  (JNIEnv *env, jobject obj)
  {
        std::string imgPath = _detector.getImgPath();
        jstring retstr = env->NewStringUTF(imgPath.c_str());
	return retstr;
  }
 
//获取输出信息 
JNIEXPORT jint JNICALL Java_com_ysten_camera_MotionDetect_getgetRectInfo
  (JNIEnv *env, jobject obj, jintArray corrArray, jintArray gameRectArray, jintArray gameRspArray)
  {
        cv::Rect celibRect;
        vector<cv::Rect> gameRects;
        vector<int> gameResponds;
         
        _detector.getRectInfo(celibRect, gameRects, gameResponds);        
          
        //将原始输入的区域转换为rect形式
        jint * pCor = (env)->GetIntArrayElements(corrArray, 0 );
        jint * pRect = (env)->GetIntArrayElements(gameRectArray, 0 );
        jint * pRsp = (env)->GetIntArrayElements(gameRspArray, 0 );
        pCor[0] = celibRect.x;
        pCor[1] = celibRect.y;
        pCor[2] = celibRect.width;
        pCor[3] = celibRect.height;
        for(int i=0; i<gameRects.size(); i++)
        {
                pRect[i*4] = gameRects[i].x;
                pRect[i*4+1] = gameRects[i].y;
                pRect[i*4+2] = gameRects[i].width;
                pRect[i*4+3] = gameRects[i].height;                
                pRsp[i] = gameResponds[i];
        }
        
        
        (env)->ReleaseIntArrayElements(corrArray, pCor, 0 );
        (env)->ReleaseIntArrayElements(gameRectArray, pRect, 0 );
        (env)->ReleaseIntArrayElements(gameRspArray, pRsp, 0 ); 
        return gameRects.size();
  }
  
JNIEXPORT jint JNICALL Java_com_ysten_camera_MotionDetect_destory
  (JNIEnv *env, jobject obj)
  {
          int ret = _detector.destory();
          return ret;
  }
  