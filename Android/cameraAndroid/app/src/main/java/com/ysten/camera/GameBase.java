package com.ysten.camera;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import com.ysten.camera.MotionDetect;

import android.graphics.ImageFormat;
import android.graphics.Rect;
import android.graphics.YuvImage;
import android.util.Log;
import com.google.gson.Gson;
import org.apache.http.HttpEntity;
import org.apache.http.entity.ContentType;
import org.apache.http.entity.mime.HttpMultipartMode;
import org.apache.http.entity.mime.MultipartEntityBuilder;
import cn.zeki.flat.*;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.nio.charset.Charset;
import java.util.Random;

import static java.lang.Math.max;
import static java.lang.Math.min;

public class GameBase {
    private int previewW;
    private int previewH;
    private int screenW;
    private int screenH;

    //用于存储相机矫正的光斑
    private int celibRectCount = 0;
    private int[] CelibRects  = new int[500];
    private int celibIdx = 0;  //当前用哪个框
    private int[] valCelibRects = new int[400];
    private int valCelibRectCount = 0;

    //框的颜色
    private int colorRectCount = 262144;
    private int[] colorRectsVal = new int[colorRectCount*3];
    private int realColorCount = 0;
    private int precColorCount = 0;

    //用于存储游戏的框
    private int gameRectNum = 0;
    private int[] GameRects = new int[5*4];
    private int[] GameRespondCounts = new int[5];

    //多变形顶点
    private  int vert_count = 4;
    private  float[] vertx = new float[vert_count];
    private  float[] verty = new float[vert_count];

    //上次变形的时刻
    private int lastChangeFrameStamp = 0;

    private Random rand = new Random();

    public void setPreviewSize(int w ,int h) {
        this.previewW = w;
        this.previewH = h;
    }

    public void setScreenSize(int w ,int h) {
        this.screenW = w;
        this.screenH = h;
    }

    public int copyRects(int[]src, int[] dst, int src_startId, int dst_startId, int count){
        for(int i=0; i<count; i++) {
            int srcId = src_startId + i;
            int dstId = dst_startId + i;
            dst[dstId * 4] = src[srcId * 4];
            dst[dstId * 4 + 1] = src[srcId * 4 + 1];
            dst[dstId * 4 + 2] = src[srcId * 4 + 2];
            dst[dstId * 4 + 3] = src[srcId * 4 + 3];
        }
        return count;
    }

    public void setValCelibRects(int[] rects, int count) {
        valCelibRectCount = count;
        int src_startId = 0;
        int dst_startId = 0;
        copyRects(rects, this.valCelibRects, src_startId, dst_startId, count);
    }

    //生成随机数
    public int RandNumber(int begin, int end) {
        //int num = (int)(System.nanoTime() % (end - begin + 1));
        //int  val = num + begin;
        //int val = (int)( Math.random() * (end - begin + 1) ) + begin;
        int val = rand.nextInt(end - begin + 1) + begin;
        return val;
    }

    public int  isValidRect(int[] rect)
    {
        int x1 = rect[0];
        int x2 = rect[0] + rect[2];
        int y1 = rect[1];
        int y2 = rect[1] + rect[3];
        if (x1 < 0 || x1 > previewW - 1) return 0;
        if (x2 < 0 || x2 > previewW - 1) return 0;
        if (y1 < 0 || y1 > previewH - 1) return 0;
        if (y2 < 0 || y2 > previewH - 1) return 0;
        return 1;
    }

    //生成矫正用的光斑，光斑个数为celibRectCount个
    public void generateCelibRects()  {
        int  grid_y = 10;
        int  grid_x = 10;
        int  grid_w = previewW / grid_x;
        int  grid_h = previewH / grid_y;
        int  min_x = grid_w / 2;
        int  max_x = previewW - 1;
        int  min_y = grid_h / 2;
        int  max_y = previewH - 1;
        celibRectCount = 0;
        for (int y = min_y; y < max_y; y += grid_h)
        {
            for (int x = min_x; x < max_x; x += grid_w)
            {
                int[] rect = new int[4];
                rect[0] = x - grid_w / 2;
                rect[1] = y - grid_h / 2;
                rect[2] = grid_w;
                rect[3] = grid_h;
                if (isValidRect(rect) == 1)
                {
                    CelibRects[celibRectCount * 4] = x - grid_w / 2;
                    CelibRects[celibRectCount * 4+1] = y - grid_h / 2;
                    CelibRects[celibRectCount * 4+2] = grid_w;
                    CelibRects[celibRectCount * 4+3] = grid_h;
                    celibRectCount = celibRectCount + 1;
                }
            }
        }
        CelibRects[celibRectCount * 4] = 0;
        CelibRects[celibRectCount * 4+1] = 0;
        CelibRects[celibRectCount * 4+2] = 0;
        CelibRects[celibRectCount * 4+3] = 0;
        celibRectCount = celibRectCount + 1;

        //产生所有颜色
        generateColors();
    }

    //产生所有颜色
    public int generateColors()  {
        int step = 64;
        int  count = 0;
        for(int b = 0; b<256; b += step){
            for(int g = 0; g<256; g += step){
                for(int r = 0; r<256; r += step){
                    colorRectsVal[count*3] = b;
                    colorRectsVal[count*3+1] = g;
                    colorRectsVal[count*3+2] = r;
                    count = count + 1;
                }
            }
        }
 /*
        colorRectsVal[count*3] = 0;
        colorRectsVal[count*3+1] = 0;
        colorRectsVal[count*3+2] = 255;
        count = count + 1;
*/
        realColorCount = count;
        return count;
    }

    //判断点是否在多边形内部
    public int pnpoly(float testx, float testy)    {
        int nvert = 4;
        int i, j, c = 0;
        for (i = 0, j = nvert - 1; i < nvert; j = i++)
        {
            if (((verty[i]>testy) != (verty[j]>testy)) &&
                    (testx < (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) + vertx[i]))
                c = 1 - c;
        }
        return c;
    }

    //判断当前框是否在多边形内部
    public int rectInRange(int[] rect)
    {
        //依次判断每个点是否都在多边形内部
        int ret = 0;
        float  x1 = rect[0];
        float  y1 = rect[1];
        ret = pnpoly(x1, y1);
        if (ret == 0)
            return 0;

        float  x2 = rect[0]+rect[2];
        float  y2 = rect[1];
        ret = pnpoly(x2, y2);
        if (ret == 0)
            return 0;

        float  x3 = rect[0]+rect[2];
        float  y3 = rect[1]+rect[3];
        ret = pnpoly(x3, y3);
        if (ret == 0)
            return 0;

        float  x4 = rect[0];
        float  y4 = rect[1]+rect[3];
        ret = pnpoly(x4, y4);
        if (ret == 0)
            return 0;

        return 1;
    }

    //随机生成一个框
    public void  randRect(int[] rect)
    {
        int  grid_y = 11;
        int  grid_x = 11;
        int  grid_w = previewW / grid_x;
        int  grid_h = previewW / grid_x;

        int  min_x = grid_w;
        int  max_x = previewW - grid_w;
        int  min_y = grid_h;
        int  max_y = previewH - grid_h;

        int cx = RandNumber(min_x, max_x);
        int cy = RandNumber(min_y, max_y);
        int x = cx - grid_w / 2;
        int y = cy - grid_h / 2;

        rect[0] = x;
        rect[1] = y;
        rect[2] = grid_w;
        rect[3] = grid_h;
    }

    //计算两个矩形框的重叠度
    public float getRectOverlap(int[] rect1, int[] rect2)
    {
        if (rect1[2] <= 0 || rect1[3] <= 0 || rect2[2] <= 0 || rect2[3] <= 0)
        {
            return -1.0f;
        }
        float intersection, area1, area2;
        int overlapedWidth = min(rect1[0] + rect1[2], rect2[0] + rect2[2]) - max(rect1[0], rect2[0]);
        int overlapedHeight = min(rect1[1] + rect1[3], rect2[1] + rect2[3]) - max(rect1[1], rect2[1]);
        intersection = overlapedWidth * overlapedHeight;
        if (intersection <= 0 || overlapedWidth <= 0 || overlapedHeight <= 0)
            return 0.0f;
        area1 = rect1[2] * rect1[3];
        area2 = rect2[2] * rect2[3];
        return intersection / (area1 + area2 - intersection);
    }

    //是否重叠
    public int  isOverlapRect(int[] totalRects, int count, int[] curRect)
    {
        for (int i = 0; i < count; i++)
        {
            int[] rectTmp = new int[4];
            copyRects(totalRects, rectTmp, i, 0, 1);
            float ov = getRectOverlap(rectTmp, curRect);
            if (ov > 0.1 || ov < 0)
            {
                return 0;
            }
        }
        return 1;
    }

    /*
    //生成一个既不与其它框重叠，同时也不能在运动区域
    public void  generateRandRect(MotionDetect _motion, int[] curRect)
    {
        int cycCounter = 0;
        int minDiff = 255;
        int[] curRectTmp = new int[4];
        int[] respond = new int[1];
        while (true)
        {
            randRect(curRect);
            int ret = isOverlapRect(GameRects, gameRectNum, curRect);  //这个框是否与其它框重叠
            _motion.getResponds(curRect, respond, 1);  //这个框是否还是在运动比例
            int inPoly = rectInRange(curRect);  //判断这个框是否在多边形内部
            if (ret == 1 && respond[0] < 6 && inPoly == 1)  //只有当这个框不与其它框重叠，并且同时也不在运动区域，则这个框才有效
            {
                int diffCount = _motion.getRectSim(curRect);  //提取当前框中颜色与背景颜色的相似度
                //保存diff最小的
                if(diffCount < minDiff) {
                    minDiff = diffCount;
                    copyRects(curRect, curRectTmp, 0, 0, 1);
                }
                if(diffCount < 5 || cycCounter > 10) {
                    break;
                }
                else {
                    cycCounter = cycCounter + 1;
                }
            }
        }

        Log.i("generate", "cycCount: "+cycCounter+", diff: "+minDiff);
        //最后把diff最小的作为输出
        copyRects(curRectTmp, curRect, 0, 0, 1);
    }

    //生成游戏用的所有框
    public void  generateGameRects(int state, MotionDetect  _motion)
    {
        if (state != 6)  //camera_state::celib_over
            return;
        if(gameRectNum == 5)  //游戏框只生成一次
            return;
        gameRectNum = 0;
        for (int i = 0; i < 5; i++)
        {
            int[] curRect = new int[4];
            generateRandRect(_motion, curRect);

            copyRects(curRect, GameRects, 0, i, 1);
            GameRespondCounts[i] = 0;
            gameRectNum = gameRectNum + 1;
            //Log.i("GameRects", "rect x1:" + curRect[0] + ",y1:" + curRect[1] + ",x2:"+ curRect[2] + ",y2:" + curRect[3]);
        }
    }

    //提取每个框的响应，并对大的响应重新修改框的位置
    public int  setGameRects(int[] rects, int[] responds, int frameCounter)
    {
        copyRects(GameRects, rects, 0, 0, gameRectNum);
        //结果拷贝出去
        for (int i = 0; i < gameRectNum; i++)
        {
            responds[i] = GameRespondCounts[i];
            lastChangeFrameStamp = frameCounter;
        }
        return gameRectNum;
    }

    //提取每个框的响应，并对大的响应重新修改框的位置
    public int  respondRects(int thresh, MotionDetect  _motion, int[] rects, int[] responds, int[] respondCounts, int frameCounter)
    {
        //根据每个框里面的运动信息判断是否更新框的位置
        if(frameCounter - lastChangeFrameStamp > 10){
            for (int i = 0; i < gameRectNum; i++) {
                if (responds[i] > thresh) {
                    //Log.i("respond", "game:" + responds[i]);
                    int[] curRect = new int[4];
                    generateRandRect(_motion, curRect);

                    copyRects(GameRects, curRect, 0, i, 1);
                    GameRespondCounts[i] = GameRespondCounts[i] + 1;
                    lastChangeFrameStamp = frameCounter;
                }
            }
        }


        //结果拷贝出去
        copyRects(GameRects, rects, 0, 0, gameRectNum);
        for (int i = 0; i < gameRectNum; i++)
        {
            respondCounts[i] = GameRespondCounts[i];
        }
        return gameRectNum;
    }

    //设置运动范围
    public void  setMotionRange(int[] range)
    {
        for (int i = 0; i < 4; i++)
        {
            vertx[i] = (float)range[i*2];
            verty[i] = (float)range[i*2+1];
        }
    }

    //根据状态决定是否打光斑，并决定输出哪个框
    public int  celibRun(int state, int[] celibRect, int rectId)
    {
        celibIdx = rectId;
        if(celibIdx >= celibRectCount-1) {
            celibIdx = celibRectCount - 1;
        }

        int ret = 0;
        if (state == 3)  //如果开始匹配，则打光斑  camera_state::start_match
        {
            ret = 1;
        }

        copyRects(CelibRects, celibRect, celibIdx, 0, 1);

        //Log.i("celibRun","celibIdx:" + celibIdx+ ", state:"+ state + ", setPlot:" + ret);
        return ret;
    }

    public int getCelibRects(int[] rects){

        copyRects(CelibRects, rects, 0, 0,celibRectCount);

        return  celibRectCount;
    }


    //生成色板
    public int generateColorRects(int[] colorRects, int[] colorVal)  {
        int colorRectCount = 0;
        if(precColorCount >= realColorCount)
            return colorRectCount;

        int rectCount = 0;
        int[] curRect = new int[4];
        for(int i=0; i<valCelibRectCount-1; i++){
            //看当前rect是否有效
            copyRects(valCelibRects, curRect, i, 0, 1);
            if (rectInRange(curRect) == 1)  //判断这个框是否在多边形内部
            {
                //拷贝矩形框
                copyRects(curRect, colorRects, 0, rectCount, 1);

                //矩形框内的颜色
                colorVal[rectCount*3] = colorRectsVal[precColorCount*3];
                colorVal[rectCount*3+1] = colorRectsVal[precColorCount*3+1];
                colorVal[rectCount*3+2] = colorRectsVal[precColorCount*3+2];

                precColorCount = precColorCount + 1;
                rectCount = rectCount + 1;
            }
        }

        colorRectCount = rectCount;
        return colorRectCount;
    }

*/

    public byte[] readToString(String fileName) {
        String encoding = "UTF-8";
        File file = new File(fileName);
        Long filelength = file.length();
        byte[] filecontent = new byte[filelength.intValue()];
        try {
            FileInputStream in = new FileInputStream(file);
            in.read(filecontent);
            in.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }

        return  filecontent;
    }


    public static YuvImage convertNV21ToYuvImage(byte[] nv21, int width, int height) {
        int format  = ImageFormat.NV21;
        YuvImage yuvImage = new YuvImage(nv21, format, width, height, null);
        return yuvImage;
    }

    public void adjustRectangle(Rect rect) {
        int width = rect.width();
        int height = rect.height();
        // Make sure left, top, width and height are all even.
        width &= ~1;
        height &= ~1;
        rect.left &= ~1;
        rect.top &= ~1;
        rect.right = rect.left + width;
        rect.bottom = rect.top + height;
    }

    public byte[] convertYuvToJpgByteArray(YuvImage yuvImage, Rect rect) {
        int quality = 100;
        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
        yuvImage.compressToJpeg(rect, quality, outputStream);
        return outputStream.toByteArray();
    }

    public byte[]  showCamera(byte[] nv21, int width, int height, Rect rect)   {
        YuvImage yuv = convertNV21ToYuvImage(nv21, width, height);
        return convertYuvToJpgByteArray(yuv, rect);
    }

    //网络发送信息
    public String runNodes(byte[] data)
    {
        MultipartEntityBuilder builder = MultipartEntityBuilder.create();
        builder.setCharset(Charset.forName("utf-8"));//设置请求的编码格式
        builder.setMode(HttpMultipartMode.BROWSER_COMPATIBLE);//设置浏览器兼容模式
        String fileName = String.format("face_%d.jpg", System.currentTimeMillis());
        String SERVER_URL = "http://172.22.11.28/api";
        // image data
        builder.addBinaryBody("file", data, ContentType.APPLICATION_OCTET_STREAM, fileName);
        // nodes
        String param = "warp";//merge(nodeName, ";");
        builder.addTextBody("json", param);
        // type dim
        //builder.addTextBody("type", Integer.toString(type));
        //builder.addTextBody("width", Integer.toString(width));
        //builder.addTextBody("height", Integer.toString(height));
        // 发起请求 并返回请求的响应
        HttpEntity entity = builder.build();
        Flat.put("postUrl", SERVER_URL);
        Flat.put("postEntity", entity);
        Flat.put("postRet", "");
        Thread t = new Thread(new Runnable() {
            @Override
            public void run() {
                String ret = Net.post(Flat.getString("postUrl"),(HttpEntity)Flat.get("postEntity"));
                Flat.put("postRet", ret);
            }
        });
        t.start();
        long delay = 5000;
        long start = System.currentTimeMillis();
        String ret="";
        boolean ok = false;
        while (System.currentTimeMillis() < start + delay) {
            ret = Flat.getString("postRet");
            if (ret != null && !ret.isEmpty())
            {
                ok = true;
                break;
            }
            try {
                Thread.sleep(200);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        if (!ok)
        {
            t.interrupt();
        }
        return ret;
    }
}
