package com.ysten.camera;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Rect;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.Window;
import android.view.WindowManager;

import java.util.List;

import com.ysten.test.R;

import static java.lang.Math.max;
import static java.lang.Math.min;

public class MainActivity extends Activity implements SurfaceHolder.Callback {

    private static final String TAG = "MainActivity";

    //private String filePath = Environment.getExternalStorageDirectory().getPath() + "/faceDetect";

    private static Context context = null;
    private  RectView mRectView = null;
    private SurfaceView mSurfaceview = null;  // SurfaceView����(��ͼ���)��Ƶ��ʾ
    private SurfaceHolder mSurfaceHolder = null;  // SurfaceHolder����(����ӿ�)SurfaceView֧����
    private Camera mCamera =null;     // Camera�������Ԥ��
    private boolean bIfPreview = false;
    private int mPreviewHeight = 0;
    private int mPreviewWidth = 0;
    private Camera.AutoFocusCallback mAutoFocusCallback = null;
    private boolean isRefMode = false;
    private boolean isMatchMode = false;
    private MotionDetect _motionDetector = null;
    private GameBase _gamer = null;
    private int frameCount = 0;
    private long lastShowTimeStamp = 0;

    private int gameRectCount = 10;
    private int[] corRect = new int[4];
    private int[] gameRects = new int[gameRectCount*4];
    private int[] gameRectRsp = new int[gameRectCount];

    static {
        // 加载动态库
        System.loadLibrary("opencv_java");
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("motionDetect");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        /* 隐藏状态栏 */
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //隐藏标题栏
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.activity_main);
        context = this;
        initView();

        _motionDetector = new  MotionDetect();
        _gamer = new GameBase();
     }

    protected void onDestroy() {
        super.onDestroy();
        //_facial.facialKeyPointsDestory();
        isRefMode = false;
        isMatchMode = false;
        _motionDetector.destory();
    }

    // InitSurfaceView
    private void initView()
    {
        /*
        Button tpButton = (Button) findViewById(R.id.take);
        tpButton.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (mCamera != null) {
                    if (event.getAction() == MotionEvent.ACTION_UP) {
                        isRefMode = false;
                        isMatchMode = false;
                        //init_flag = 1 - init_flag;   //改变符号
                    }
                    if (event.getAction() == MotionEvent.ACTION_DOWN) {
                        isRefMode = true;
                        isMatchMode = false;
                    }
                }
                return false;
            }
        });


        Button matchButton = (Button) findViewById(R.id.match);
        matchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (mCamera != null) {
                    isMatchMode = true;
                }
            }
        });
        */
        isRefMode = false;
        mSurfaceview = (SurfaceView) this.findViewById(R.id.surfaceview);
        mSurfaceHolder = mSurfaceview.getHolder(); // ��SurfaceView��ȡ��SurfaceHolder����
        mSurfaceHolder.addCallback(MainActivity.this); // SurfaceHolder����ص��ӿ�
        mSurfaceHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);// �O���@ʾ����ͣ�setType��������
        mRectView = (RectView)this.findViewById(R.id.rectview);
        DisplayMetrics metric = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metric);
        mRectView.setScreenSize(metric.widthPixels,metric.heightPixels);
    }

    /*【SurfaceHolder.Callback 回调函数】*/
    public void surfaceCreated(SurfaceHolder holder)
    // SurfaceView启动时/初次实例化，预览界面被创建时，该方法被调用。
    {
        // TODO Auto-generated method stub
        //mCamera = Camera.open();// 开启摄像头（2.3版本后支持多摄像头,需传入参数）
        mCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK);// 开启摄像头（2.3版本后支持多摄像头,需传入参数）
        try {
            Log.i(TAG, "SurfaceHolder.Callback：surface Created");
            mCamera.setPreviewDisplay(mSurfaceHolder);//set the surface to be used for live preview
        } catch (Exception ex)
        {
            if(null != mCamera) {
                mCamera.release();
                mCamera = null;
            }
            //Log.i(TAG+"initCamera", ex.getMessage());
        }
    }
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        // TODO Auto-generated method stub
        Log.i(TAG, "SurfaceHolder.Callback：Surface Changed");
        initCamera();
        _motionDetector.init("./sdcard/DCIM/IMG/");
    }

    public void surfaceDestroyed(SurfaceHolder holder)
    // SurfaceView销毁时，该方法被调用
    {
        // TODO Auto-generated method stub
        //Log.i(TAG, "SurfaceHolder.Callback：Surface Destroyed");
        if(null != mCamera)
        {
            mCamera.setPreviewCallback(null); //！！这个必须在前，不然退出出错
            mCamera.stopPreview();
            bIfPreview = false;
            mCamera.release();
            mCamera = null;
        }
    }

    /*【2】【相机预览】*/
    private void initCamera()//surfaceChanged中调用
    {
        Log.i(TAG, "going into initCamera");
        if (bIfPreview)
        {
            mCamera.stopPreview();//stopCamera();
        }
        if(null != mCamera)
        {
            try
            {
    /* Camera Service settings*/
                Camera.Parameters parameters = mCamera.getParameters();
                List<Camera.Size> mSupportedPreviewSizes = mCamera.getParameters()
                        .getSupportedPreviewSizes();
                Camera.Size optsize = getOptimalPreviewSize(mSupportedPreviewSizes, 1280, 720);
                parameters.setPreviewSize(optsize.width, optsize.height);
                //parameters.setPreviewFormat(ImageFormat.NV21);
                mPreviewWidth = parameters.getPreviewSize().width;
                mPreviewHeight = parameters.getPreviewSize().height;
// 横竖屏镜头自动调整
                if (this.getResources().getConfiguration().orientation != Configuration.ORIENTATION_LANDSCAPE)
                {
                    Log.i("motion:", "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~camera1");
                    parameters.set("orientation", "portrait"); //
                    parameters.set("rotation", 90); // 镜头角度转90度（默认摄像头是横拍）
                    mCamera.setDisplayOrientation(90); // 在2.2以上可以使用
                } else// 如果是横屏
                {
                    Log.i("motion:", "*******************************camera2");
                    parameters.set("orientation", "landscape"); //
                    mCamera.setDisplayOrientation(0); // 在2.2以上可以使用
                }

     /* 视频流编码处理 */
                //添加对视频流处理函数
                mCamera.setPreviewCallback(mYUVPreviewCallback);
// 设定配置参数并开启预览
                mCamera.setParameters(parameters); // 将Camera.Parameters设定予Camera
                mCamera.startPreview(); // 打开预览画面
                mRectView.setPreviewSize(mPreviewWidth, mPreviewHeight);
                bIfPreview = true;
            } catch (Exception e)
            {
                e.printStackTrace();
            }
        }
    }

    private Camera.Size getOptimalPreviewSize(List<Camera.Size> sizes, int w, int h) {
        final double ASPECT_TOLERANCE = 0.1;
        double targetRatio = (double) w / h;
        if (sizes == null)
            return null;

        Camera.Size optimalSize = null;
        double minDiff = Double.MAX_VALUE;

        int targetHeight = h;

        // Try to find an size match aspect ratio and size
        for (Camera.Size size : sizes) {
            double ratio = (double) size.width / size.height;
            if (Math.abs(ratio - targetRatio) > ASPECT_TOLERANCE)
                continue;
            if (Math.abs(size.height - targetHeight) < minDiff) {
                optimalSize = size;
                minDiff = Math.abs(size.height - targetHeight);
            }
        }

        // Cannot find the one match the aspect ratio, ignore the requirement
        if (optimalSize == null) {
            minDiff = Double.MAX_VALUE;
            for (Camera.Size size : sizes) {
                if (Math.abs(size.height - targetHeight) < minDiff) {
                    if((size.height - targetHeight)<0){
                        optimalSize = size;
                        minDiff = Math.abs(size.height - targetHeight);
                    }
                }
            }
        }
        return optimalSize;
    }

    // ����ȡ��ƵԤ��֡�Ľӿڡ�
    private Camera.PreviewCallback mYUVPreviewCallback = new Camera.PreviewCallback() {
        public void onPreviewFrame(byte[] data, Camera arg1) {

            frameCount = frameCount + 1;
            int num=0;
            long startTime = System.nanoTime() / 1000000 ;  //開始時間，毫秒

            //获取矫正的状态
            int  state = _motionDetector.getState();

            _motionDetector.run(data, mPreviewWidth, mPreviewHeight);

            int rectNum = _motionDetector.getgetRectInfo(corRect, gameRects, gameRectRsp);

            String imgPath = _motionDetector.getImgPath();

            //重新调整框的位置
            long consumingTime = System.nanoTime() / 1000000 - startTime; //消耗時間
            //Log.i("motion:", "java time =" + consumingTime + "ms");

            //显示信息
            mRectView.setDanceInfos(corRect, gameRects, gameRectRsp, rectNum, state, imgPath);

            long diff = startTime - lastShowTimeStamp;

            /*
            //显示图片
            if(diff > 1000* 1){
                byte[] imgData = _gamer.readToString("./sdcard/DCIM/xj/test.jpg");  //显示保存图片
                _gamer.runNodes(imgData);
                lastShowTimeStamp = System.nanoTime() / 1000000 ;
            }*/
            /*
            Rect  rect = new Rect(0,0,mPreviewWidth,mPreviewHeight);
            if(diff > 1000* 5)  {   //10秒显示一次
                byte[] imgData = _gamer.showCamera(data, mPreviewWidth, mPreviewHeight,rect);   //显示摄像头
                _gamer.runNodes(imgData);
                lastShowTimeStamp = System.nanoTime() / 1000000 ;
            }*/

            mRectView.invalidate();

            arg1.startPreview(); //重新开始预览*/
        }
    };
}
