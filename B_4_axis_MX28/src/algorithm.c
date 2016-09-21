#include "algorithm.h"
#include "dmath.h"
#include <stdio.h>
#include <math.h>

//逆运动学变量
static float Dxyzboard[3]; //端点D在棋盘上的坐标
static float Dxyzabs[3];   //端点D在绝对坐标系中的坐标
static float AxyzO[3];     //点A在O坐标系中的坐标
static float jointradian[3];//关节弧度

//逆运动学速度变量
static float Dspeboard[3]; //点D在棋盘的速度向量
static float AspeO[3]; //点A在O系中的速度分量
static float jointsperadian[3]; //关节弧速度

//设置点D在棋盘坐标
//输入的是点D在棋盘中的坐标
void set_Dxyzboard(float x, float y, float z)
{
    Dxyzboard[0] = x; Dxyzboard[1] = y; Dxyzboard[2] = z;
}

//设置点D在绝对坐标系中的坐标
//输入的是点D在绝对坐标系中的坐标
void set_Dxyzabs(float x, float y, float z)
{
    Dxyzabs[0] = x; Dxyzabs[1] = y; Dxyzabs[2] = z;
}

//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
void trans_Dxyzboard_Dxyzabs()
{
    Dxyzabs[0] = Dxyzboard[0] * GRIDW;

    if(Dxyzboard[1] <= 4.0) {
        Dxyzabs[1] = Dxyzboard[1] * GRIDH + DISBOUNDARY;
    }
    else if((Dxyzboard[1] <= 5.0) && (Dxyzboard[1] > 4.0)) {
        Dxyzabs[1] = 4 * GRIDH + DISBOUNDARY;
        Dxyzabs[1] = Dxyzabs[1] + (Dxyzboard[1] - 4.0) * MIDH;
    }
    else {
        Dxyzabs[1] = (Dxyzboard[1] - 1) * GRIDH + DISBOUNDARY;
        Dxyzabs[1] = Dxyzabs[1] + MIDH;
    }

    Dxyzabs[2] = Dxyzboard[2] - DIFFHEIGHT;
}

//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
void trans_Dxyzabs_AxyzO()
{
    float tan_alpha, alpha, temp;
    float Oxyzabs[3];
    float L_do, mDY, mDZ, mAY, mAZ;

    //计算中轴关节偏0线的角度
    tan_alpha = Dxyzabs[0] / Dxyzabs[1];
    alpha = atan(tan_alpha);

    //计算O在绝对坐标系中的坐标
    Oxyzabs[0] = D0 * sin(alpha);
    Oxyzabs[1] = D0 * cos(alpha);
    Oxyzabs[2] = LA;

    //计算目标点D与点O的距离
    temp = pow(Dxyzabs[0] - Oxyzabs[0], 2);
    temp = temp + pow(Dxyzabs[1] - Oxyzabs[1], 2);
    temp = temp + pow(Dxyzabs[2] - Oxyzabs[2], 2);
    L_do = sqrt(temp);

    //计算点D在坐标系O中的坐标值
    mDZ = Dxyzabs[2] - LA;
    temp = pow(L_do, 2) - pow(mDZ, 2);
    mDY = sqrt(temp);

    //计算点A在坐标系O中的坐标值
    mAY = mDY - D1;
    mAZ = mDZ + D2;

    //返回点A在坐标系O中的坐标值
    AxyzO[0] = alpha;
    AxyzO[1] = mAY;
    AxyzO[2] = mAZ;
}

//点A在O系中的坐标转化为关节弧度
//计算出关节弧度
void cal_AxyzO_jointradian()
{
    float temp, temp1, beta, gamma2;
    float LAO, beta1, cos_beta1, deltaBeta;
    float tan_deltaBeta, alpha, cos_alpha;

    //计算LAO的距离
    temp = pow(AxyzO[1], 2) + pow(AxyzO[2], 2);
    LAO = sqrt(temp);

    //计算beta的角度
    temp = pow(LB, 2) + pow(LAO, 2) - pow(LC, 2);
    temp1 = 2 * LB * LAO;
    cos_beta1 = temp / temp1;
    beta1 = acos(cos_beta1);

    tan_deltaBeta = AxyzO[2] / AxyzO[1];
    deltaBeta = atan(tan_deltaBeta);

    beta = beta1 + deltaBeta;

    //计算gamma2
    temp = pow(LB, 2) + pow(LC, 2) - pow(LAO, 2);
    temp1 = 2 * LC * LB;
    cos_alpha = temp / temp1;
    alpha = acos(cos_alpha);

    gamma2 = PI - beta - alpha;

    //返回此时机械臂关节弧度
    jointradian[0] = AxyzO[0];
    jointradian[1] = beta;
    jointradian[2] = gamma2;
}

//获取关节弧度
void get_jointradian(float jointrad[])
{
    jointrad[0] = jointradian[0];
    jointrad[1] = jointradian[1];
    jointrad[2] = jointradian[2];
}

//逆运动学速度求解
//static float Dspeboard[3]; //点D在棋盘的速度向量
//static float AspeO[3]; //点A在O系中的速度分量
//static float jointsperadian[3]; //关节弧速度
//设置点D在棋盘中的速度
void set_Dspeboard(float start[], float end[], float t)
{
    Dspeboard[0] = (start[0] - end[0]) / t;
    Dspeboard[1] = (start[1] - end[1]) / t;
    Dspeboard[2] = (start[2] - end[2]) / t;
}

//设置点A在O系中的速度
void set_AspeO(float start[], float end[], float t)
{
    float AxyzO_start[3], AxyzO_end[3];

    //把点D在棋盘中的坐标换算成点A在O系中的坐标
    set_Dxyzboard(start[0], start[1], start[2]); //设置点D在棋盘中的坐标
    trans_Dxyzboard_Dxyzabs(); //点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO(); //点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    AxyzO_start[0] = AxyzO[0];
    AxyzO_start[1] = AxyzO[1];
    AxyzO_start[2] = AxyzO[2];

    set_Dxyzboard(end[0], end[1], end[2]); //设置点D在棋盘中的坐标
    trans_Dxyzboard_Dxyzabs(); //点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO(); //点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    AxyzO_end[0] = AxyzO[0];
    AxyzO_end[1] = AxyzO[1];
    AxyzO_end[2] = AxyzO[2];

    //计算点A在O系中的速度
    AspeO[0] = (AxyzO_end[0] - AxyzO_start[0]) / t; //中轴的弧速度
    AspeO[1] = (AxyzO_end[1] - AxyzO_start[1]) / t;
    AspeO[2] = (AxyzO_end[2] - AxyzO_start[2]) / t;
}

//static float Dspeboard[3]; //点D在棋盘的速度向量
//static float AspeO[3]; //点A在O系中的速度分量
//static float jointsperadian[3]; //关节弧速度
//求出关节弧速度
void cal_jointsperadian()
{
    float m1, n1, m2, n2;
	float temp1, temp2;

    //计算临时参数m1, n1, m2, n2
    //jointradian:表示当前关节弧度
	m1 = LB * sin(jointradian[1]);
	n1 = LC * sin(jointradian[2]);
	m2 = LB * cos(jointradian[1]);
	n2 = LC * cos(jointradian[2]);

    //计算关节弧速度
	temp1 = n1 * AspeO[2] - n2 * AspeO[1];
	temp2 = m1 * n2 + m2 * n1;
	jointsperadian[1] = temp1 / temp2;

	temp1 = -(m2 * AspeO[1] + m1 * AspeO[2]);
	temp2 = m2 * n1 + m1 * n2;
	jointsperadian[2] = temp1 / temp2;

    //计算中轴弧速度
    jointsperadian[0] = AspeO[0];
}

 //获取关节弧速度
void get_jointsperadian(float jointsperad[])
{
    jointsperad[0] = jointsperadian[0];
    jointsperad[1] = jointsperadian[1];
    jointsperad[2] = jointsperadian[2];
}
