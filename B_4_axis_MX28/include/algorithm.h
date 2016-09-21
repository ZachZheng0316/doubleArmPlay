#ifndef _ALGORITHM_HEADER
#define _ALGORITHM_HEADER

#ifdef __cplusplus
extern "C" {
#endif

//硬件尺寸
#define LA (65.45) //B:A轴的长度91.08- 22.60 - 2.95
#define LB (179.68) //B轴的长度
#define LC (179.68) //C轴的长度
#define D0 (38.00)  //A轴中心与B轴C轴中心的距离
#define D1 (30.00)  //C轴末端与执行端的距离
#define D2 (53.700) //执行端的高度
#define HEIGHT_PRAGRAM (37.72) //常用的高度数据

#define BOARDH  (257.00)        //长度长度
#define BOARDW  (257.00)        //棋盘宽度
#define GRIDH   (28.000)        //棋盘格的纵轴长度
#define GRIDW   (32.000)        //棋盘格的横轴宽度
#define MIDH    (32.000)        //棋盘楚河汉界的宽度
#define DIFFHEIGHT  (22.680)    //B:棋盘面和机械底面的高度差
#define LineWidth   (1.0000) //线宽
#define DISBOUNDARY (141.5)  //R:棋盘底部边界到中轴的最短距离126.5 + 15.0

//逆运动学位置求解
void set_Dxyzboard(float x, float y, float z);//设置点D在棋盘坐标
void set_Dxyzabs(float x, float y, float z);  //设置点D在绝对坐标系中的坐标
void trans_Dxyzboard_Dxyzabs();               //点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
void trans_Dxyzabs_AxyzO();                   //点D在绝对坐标系中的坐标转化为点A在O系中的坐标
void cal_AxyzO_jointradian();                 //点A在O系中的坐标转化为关节弧度

void get_jointradian(float jointrad[]);       //获取关节弧度

//逆运动学速度求解
void set_Dspeboard(float start[], float end[], float t); //设置点D在棋盘中的速度
void set_AspeO(float start[], float end[], float t);     //设置点A在O系中的速度
void cal_jointsperadian();                               //求出关节弧速度

void get_jointsperadian(float jointsperad[]); //获取关节弧速度

#ifdef __cplusplus
}
#endif

#endif
