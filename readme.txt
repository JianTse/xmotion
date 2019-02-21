

该工程为投影游戏，
原理：通过检查画面中的给定区域是否运动来判断该块是否响应
算法耗时：在x3盒子上速度为50Hz

所需设备：盒子x1，投影仪x1

使用方法：
硬件配置
1：调整盒子摄像头和投影仪影像基本对齐，不要有较大的旋转角度
2：将盒子HDMI输出到投影仪显示

软件配置
1：设置背景图片，在盒子给定路径放入投影背景图片，分辨率为（1280x720），默认路径为"./sdcard/DCIM/IMG/"
2：相机矫正，查看背景图片目录下是否有match.txt文件，如果有请删除，删除后相机会重新矫正；否则按照match.txt中给定的信息进行矫正
3：启动程序，如果是矫正过程，请保证投影画面和摄像头前面没有遮挡，矫正完成后游戏画面会自动启动。


游戏配置
如果需要根据算法提供的功能配置特定游戏，需要修改androi端代码
具体请看MotionDetect类的接口函数，例子在RectView.java中的onDraw函数

//初始化接口，输入背景图片路径,demo中的路径为："./sdcard/DCIM/IMG/"
int init(String imgDir);  

//执行主体
int run(byte[] data, int width, int height);  

//获取状态，如果状态为2：画矫正光斑，如果为6：显示背景图片，其他：显示黑色背景
int getState();   

//现在android需要显示背景图片的路径，
String getImgPath();  

//输出框的信息corRect：矫正光斑，gameRects：游戏块区域，responds：每个游戏块的响应次数
int getgetRectInfo(int[] corRect, int[] gameRects, int[] responds);  

int destory();
