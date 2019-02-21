package com.ysten.camera;

/**
 * Created by Administrator on 2016/3/8.
 */
public class MotionDetect {

    public native int init(String imgDir);

    public native int run(byte[] data, int width, int height);

    public native int getState();

    public native String getImgPath();

    public native int getgetRectInfo(int[] corRect, int[] gameRects, int[] responds);

    public native int destory();
 
}
