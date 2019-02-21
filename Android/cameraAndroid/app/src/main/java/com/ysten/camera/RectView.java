package com.ysten.camera;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.util.AttributeSet;
import android.util.Log;
import android.util.TypedValue;
import android.view.View;

import com.ysten.test.R;

import java.io.File;

/**
 * Created by Administrator on 2015/12/2.
 */
public class RectView extends View {
    private Paint mPaint;
    private int previewW;
    private int previewH;
    private int screenW;
    private int screenH;
    private int m_curState = 0;

    private int rectCount = 0;
    private int rectCounts = 10;
    private int[] m_celibRect = new int[4];
    private int[] m_gameRects = new int[rectCounts*4];
    private int[] m_gameRectRsp  = new int[rectCounts];
    private int[] colors = new int[rectCounts+2];
    private int flip = 0;

    private Bitmap girlBitmap = null;
    private int girlBitWidth , girlBitHeight;
    private Rect girlSrcRect , girlDesRect;
    String m_imagePath, m_lastImgPath;

    public boolean fileIsExists(String strFile)
    {
        try
        {
            File f=new File(strFile);
            if(!f.exists())
            {
                return false;
            }
        }
        catch (Exception e)
        {
            return false;
        }
        return true;
    }

    public void readBitmap() {
        //读背景图片
        if(fileIsExists(m_imagePath)) {
            if(!m_imagePath.equals(m_lastImgPath)) {
                Log.i("draw", "last bitmap: " + m_lastImgPath);
                Log.i("draw", "cur bitmap: " + m_imagePath);

                girlBitmap = BitmapFactory.decodeFile(m_imagePath);
                girlBitWidth = girlBitmap.getWidth();
                girlBitHeight = girlBitmap.getHeight();
                girlSrcRect = new Rect(0, 0, girlBitWidth, girlBitHeight);
                girlDesRect = new Rect(0, 0, screenW, screenH);
                m_lastImgPath = m_imagePath;
            }
        }
        else{
            Log.i("draw", "bitmap not exist: " + m_imagePath);
        }
    }

    public int copyRects(int[]src, int[] dst, int src_startId, int dst_startId, int count) {
        for (int i = 0; i < count; i++) {
            int srcId = src_startId + i;
            int dstId = dst_startId + i;
            dst[dstId * 4] = src[srcId * 4];
            dst[dstId * 4 + 1] = src[srcId * 4 + 1];
            dst[dstId * 4 + 2] = src[srcId * 4 + 2];
            dst[dstId * 4 + 3] = src[srcId * 4 + 3];
        }
        return count;
    }

    public RectView(Context context, AttributeSet attrs) {
        super(context, attrs);
        mPaint = new Paint(Paint.ANTI_ALIAS_FLAG);

        colors[0] = Color.argb(255, 255, 0, 0);//Color.RED;
        colors[1] = Color.argb(255, 0, 255, 0);//Color.GREEN;
        colors[2] = Color.argb(255, 255, 255, 0);//Color.YELLOW;
        colors[3] = Color.argb(255, 0, 191, 255);//Color.BLUE;
        colors[4] = Color.argb(255, 255, 165, 0);//
        colors[5] = Color.argb(255, 0, 0, 0);//
        colors[6] = Color.argb(255, 255, 255, 255);//
    }

    public  void drawFillRect(Canvas canvas, int[] rect, int colo_idx) {
        mPaint.setColor(colors[colo_idx]);
        int x1 = rect[0];
        int y1 = rect[1];
        int x2 = rect[2];
        int y2 = rect[3];
        canvas.drawRect(x1, y1, x2, y2, mPaint);
    }

    public  void drawTextInRect(Canvas canvas, int[] rect, String text) {
        //写字
        mPaint.setColor(Color.WHITE);
        mPaint.setStrokeWidth(4.0f);
        mPaint.setTextSize(150);
        mPaint.setTextAlign(Paint.Align.CENTER);

        //根据需要显示的内容，自动调整字体大小
        Rect boundsRect = new Rect();
        int availableTextViewWidth = rect[2] - rect[0];
        int mTextSize = 150;
        mPaint.getTextBounds(text, 0, text.length(), boundsRect);
        int textWidth = boundsRect.width();
        while (textWidth > availableTextViewWidth) {
            mTextSize -= 1;
            mPaint.setTextSize(mTextSize);

            mPaint.getTextBounds(text, 0, text.length(), boundsRect);
            textWidth = boundsRect.width();
        }

        //将字写在中心
        int cx = (rect[0] + rect[2]) / 2;
        int cy = (rect[1] + rect[3]) / 2;
        Paint.FontMetrics fontMetrics = mPaint.getFontMetrics();
        float top = fontMetrics.top;//为基线到字体上边框的距离,即上图中的top
        float bottom = fontMetrics.bottom;//为基线到字体下边框的距离,即上图中的bottom
        int baseLineY = (int) (cy - top/2 - bottom/2);//基线中间点的y轴计算公式
        canvas.drawText(text, cx,baseLineY,mPaint);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);

        // 设置Paint为无锯齿
        mPaint.setAntiAlias(true);
        mPaint.setStyle(Paint.Style.FILL);
        mPaint.setColor(Color.BLACK);
        //画黑色背景画布
        canvas.drawRect(0, 0, screenW, screenH, mPaint);

        //如果需要打光斑
        if(this.m_curState == 2){
            drawFillRect(canvas, m_celibRect, 6);
        }

        //如果已经矫正好，开始游戏
        if(this.m_curState == 6){
            //画背景图片
            if(girlBitmap != null) {
                canvas.drawBitmap(girlBitmap, girlSrcRect, girlDesRect, null);
            }

            //画游戏框，这里可以替换为其他的游戏块，例如小动物之类的
            for(int i=0; i<rectCount; i++) {
                int[] rect = new int[4];
                rect[0] = this.m_gameRects[i*4];
                rect[1] = this.m_gameRects[i*4+1];
                rect[2] = this.m_gameRects[i*4+2];
                rect[3] = this.m_gameRects[i*4+3];
                drawFillRect(canvas, rect, i);
                //Log.i("draw", "rect x1:" + rect[0] + ",y1:" + rect[1] + ",x2:"+ rect[2] + ",y2:" + rect[3]);
                String text = this.m_gameRectRsp[i] + "";
                drawTextInRect(canvas, rect, text);
            }
        }
        super.onDraw(canvas);
    }

    public void setPreviewSize(int w ,int h) {
        this.previewW = w;
        this.previewH = h;
    }

    public void setScreenSize(int w ,int h) {
        this.screenW = w;
        this.screenH = h;
    }

    public void convPoint(int[] src, int[] dst, int flip) {
        float sx = (float) screenW / previewW;
        float sy = (float) screenH / previewH;
        int x = src[0] ;
        int y = src[1] ;
        if(flip == 1){
            dst[0] = (int)(screenW - x * sx);
        }
        else {
            dst[0] = (int)(x * sx);
        }
        dst[1] = (int)(y * sy) ;
    }

    //将int* 转换为为屏幕分辨率
    public void convRect(int[] src, int[] dst, int flip)  {
        float sx = (float)screenW/previewW;
        float sy = (float)screenH/previewH;

        int x = src[0] ;
        int y = src[1] ;
        int w = src[2] ;
        int h = src[3] ;

        if(flip == 1) {
            int  x1 = (int)(screenW - x * sx);
            int  y1 = (int)(y * sy) ;

            int  x2 = x1 - (int)(w*sx);
            int  y2 = y1;

            int  x3 = x1;
            int  y3 = y1 + (int)(h*sy);

            int  x4 = x2;
            int  y4 = y3;

            dst[0] = x2;
            dst[1] = y2;
            dst[2] = x3;
            dst[3] = y3;
        }
        else {
            int x1 = (int) (x * sx);
            int y1 = (int) (y * sy);

            int x2 = x1 + (int) (w * sx);
            int y2 = y1;

            int x3 = x1;
            int y3 = y1 + (int) (h * sy);

            int x4 = x2;
            int y4 = y3;

            dst[0] = x1;
            dst[1] = y1;
            dst[2] = x4;
            dst[3] = y4;
        }
    }

    //画框和响应
    public void setDanceInfos(int[] celibRect,int[] gameRects,int[] gameRectRsp, int rectNum,  int state, String imgPath) {
        int[] src = new int[4];
        int[] dst = new int[4];
        this.rectCount = rectNum;
        for (int i = 0 ; i < rectNum; i++)
        {
            copyRects(gameRects, src, i, 0, 1);
            convRect(src, dst, flip);
            copyRects(dst, this.m_gameRects, 0, i, 1);
            this.m_gameRectRsp[i] = gameRectRsp[i];
        }

        //保存celibRect
        convRect(celibRect, this.m_celibRect, flip);
        this.m_curState = state;
        this.m_imagePath = imgPath;

        //读入图片
        readBitmap();
    }
}
