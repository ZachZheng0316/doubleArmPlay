#ifndef _DMATH_HEADER
#define _DMATH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//MX28AT舵机属性值(12V)
#define AngleRange	  (float)(360.0)       //舵机运动的角度范围
#define PositionRange (float)(4095.0)	    //舵机运动的刻度范围
#define PositionUnit  (float)(0.087912088)   //舵机每个刻度对应0.088°
#define SpeedRange    (float)(117.07)         //速度最大为114rpm
#define SpeedUnit     (float)(0.114437927)  //每个刻度对应0.111rpm
#define SpeedKRange	  (float)(1023.0)      //舵机速度刻度范围

//常量
#define PI (float)(3.141592653) //常数PI的值

//舵机编号
#define MXID1 (unsigned char)(1)
#define MXID2 (unsigned char)(2)
#define MXID3 (unsigned char)(3)
#define MXID4 (unsigned char)(4)
#define MXID5 (unsigned char)(5)
#define MXID7 (unsigned char)(7)
#define MXID6 (unsigned char)(6)
#define MXID8 (unsigned char)(8)
#define MXID9 (unsigned char)(9)

//4-AXIS基准角度(°)
#define ID1Angle (float)(0.00)
#define ID2Angle (float)(55.70)
#define ID3Angle (float)(90.00)

//B:4-AXIS校准数据
#define ID1POSK (2275 + 18)
#define ID2POSK (1732)
#define ID3POSK (2574)
#define ID4POSK (000)
#define ID5POSK (000)
#define ID6POSK (000)
#define ID7POSK (000)
#define ID8POSK (000)
#define ID9POSK (000)

//减速比
#define REDUCTION_RATE (float)(0.5)  //1:2
#define REDUCTION_RATE1 (float)(1.0) //第一个舵机的减速比
#define REDUCTION_RATE2 (float)(0.5) //第二个舵机的减速比
#define REDUCTION_RATE3 (float)(0.5) //第三个舵机的减速比

//移动极限(°)
#define ID1LeftLimit   (float)(-55.0) //ID1舵机上极限
#define ID1RightLimit  (float)(55.00) //ID1舵机下极限
#define ID2UpLimit     (float)(107.0) //ID2舵舵上极限
#define ID2DownLimit   (float)(2.500) //ID2舵机下极限
#define ID3UpLimit     (float)(110.0) //ID3舵机上极限
#define ID3DownLimit   (float)(3.000) //ID4舵机下极限

//工具函数
float AngleFromPI(float pi);             //弧度转角度
float PIFromAngle(float alpha);          //角度转弧度
float radianSpeedFromRPM(float rpm);     //把rpm转化为舵机弧速度
float rpmFromRadianSpeed(float rad);     //把舵机弧速度转化为rpm
float angleFromPositionK(unsigned char id, int positionK); //已知舵机当前刻度转换为关节当前角度
int positionKFromAngle(unsigned char arm_id, float angle); //已知关节当前角度转换为舵机当前刻度
int speedKFromRPM(float rpm);            //已知运动rpm转换为速度刻度
float rpmFromSpeedK(int speedK);         //已知速度刻度转换为rpm
int kFromRadianSpeed(int arm_id, float radianSpeed); //把关节弧速度转化为对应舵机速度寄存器中的刻度

void delay(int sec); //时间延迟函数
void delay_ms(float msec); //毫秒延迟函数
void delay_us(unsigned long usec); //微妙延迟函数

#ifdef __cplusplus
}
#endif

#endif
