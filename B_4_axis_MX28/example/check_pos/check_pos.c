#include "dmath.h"
#include "action.h"
#include "algorithm.h"
#include "joint.h"
#include "beng.h"
#include "serialCommuni.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//棋盘休息区节点数据
struct park_joint{
	float x;
	float y;
	float z;
};

struct chess_pos{
	int r;
	int c;
	int moveNum;
};

//全局变量
static struct park_joint chessWait[32];     //棋子停放列表
static struct park_joint chessBoard[10][9]; //10行9列

//全局函数
int initial_sys(int device, int baudNum); //系统初始化函数
int back_default_pos();					  //回到默认状态
void wait_move_stop_exten();			  //等待机械臂到到目标位置附近

int main()
{
	int i;
	float obj_xyz[3];
	int xulie[32] = {16 ,17, 18, 19, 5, 6, 7, 8, 9, 11, 12, 4, 31, 30, 29, 26,
					 0, 1, 2, 3, 21, 22, 23, 24, 25, 27, 28, 20, 15, 14, 13, 10};
	int index = 16;
	int row, col;
	
	//初始化
	if(1 != initial_sys(5, 1200)) {
		printf("error-->double_4-axis::main:initial_sys() failed\n");
		return -1;
	}
	else{
		printf("sys init success...\n");
		back_default_pos();
		getchar();
	}
	
	/*
	//运动到第一部份
	index = xulie[0];
	obj_xyz[0] = chessWait[index].x;
	obj_xyz[1] = chessWait[index].y;
	obj_xyz[2] = 37.72;
	//运动到上方
	move_to_xyz_t1(obj_xyz, 4.0);
	wait_move_stop_exten();
	getchar();
	//运动到下方
	obj_xyz[2] = chessWait[index].z;
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	getchar();
	
	//检测第一部份
	for(i = 1;i < 16; i++) {
		index = xulie[i];
		obj_xyz[0] = chessWait[index].x;
		obj_xyz[1] = chessWait[index].y;
		obj_xyz[2] = 37.72;
		//运动到上方
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		getchar();
		
		//运动到下方
		obj_xyz[2] = chessWait[index].z;
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		getchar();
	}
	obj_xyz[2] = 37.72;
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	getchar();

	//检测第二部份
	index = xulie[16];
	obj_xyz[0] = chessWait[index].x;
	obj_xyz[1] = chessWait[index].y;
	obj_xyz[2] = 37.72;
	//运动到上方
	move_to_xyz_t1(obj_xyz, 4.0);
	wait_move_stop_exten();
	getchar();
	//运动到下方
	obj_xyz[2] = chessWait[index].z;
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	getchar();
	
	//检测第一部份
	for(i = 17;i < 32; i++) {
		index = xulie[i];
		obj_xyz[0] = chessWait[index].x;
		obj_xyz[1] = chessWait[index].y;
		obj_xyz[2] = 37.72;
		//运动到上方
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		getchar();
		
		//运动到下方
		obj_xyz[2] = chessWait[index].z;
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		getchar();
	}
	obj_xyz[2] = 37.72;
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	getchar();
*/	
	
	//检测棋盘区域
	for(col = 0; col < 9; col++) {
		obj_xyz[0] = chessBoard[0][col].x;
		obj_xyz[1] = chessBoard[0][col].y;
		obj_xyz[2] = 37.72;
		//运动到上方
		move_to_xyz_t1(obj_xyz, 4.0);
		wait_move_stop_exten();
		getchar();
		
		//运动到下方
		obj_xyz[2] = chessBoard[0][col].z;
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		getchar();
		
		for(row = 1; row < 10; row++) {
			obj_xyz[0] = chessBoard[row][col].x;
			obj_xyz[1] = chessBoard[row][col].y;
			obj_xyz[2] = 37.72;
			//运动到上方
			move_to_xyz_t1(obj_xyz, 1.0);
			wait_move_stop_exten();
			getchar();
		
			//运动到下方
			obj_xyz[2] = chessBoard[row][col].z;
			move_to_xyz_t1(obj_xyz, 1.0);
			wait_move_stop_exten();
			getchar();
		}
	}
	
	return 0;
	
}

//初始化
int initial_sys(int device, int baudNum)
{
	int r, c;
	float x, y, z;
	FILE *fp = NULL;

    //打开通信串口
    if(1 != serial_open(device, baudNum)) {
        printf("double_4-axis::initail_sys:serial_open failed\n");
        return -1;
    }
     
    //打开气泵串口，并关闭气泵
    if(1 != open_port()) {
        printf("double_4-axis::initail_sys:open_port failed\n");
        return -1;
    }
    else {
    	//打开成功，则关闭气泵
    	close_beng();
    }
    
    //打开控制机械臂系统，并初始化
    if(1 != initail_system()) {
    	printf("error-->double_4-axis::initial_sys:initail_system\n");
    	return -1;
    }
    
    //棋子停放区列表初始化
    chessWait[16].x = -5.09; chessWait[16].y = 0.50; chessWait[16].z = -3.50;   
    chessWait[17].x = -5.10; chessWait[17].y = 1.78; chessWait[17].z = -3.50; 
	chessWait[18].x = -5.10; chessWait[18].y = 2.87; chessWait[18].z = -3.50;
	chessWait[19].x = -5.12; chessWait[19].y = 3.96; chessWait[19].z = -4.00; 
	chessWait[5].x = -5.120; chessWait[5].y = 4.950; chessWait[5].z = -4.000;
	chessWait[6].x = -5.140; chessWait[6].y = 5.990; chessWait[6].z = -4.300;
	chessWait[7].x = -5.160; chessWait[7].y = 7.080; chessWait[7].z = -4.300;
	chessWait[8].x = -5.170; chessWait[8].y = 8.340; chessWait[8].z = -3.100;
	chessWait[9].x = -5.880; chessWait[9].y = 7.710; chessWait[9].z = -3.500;
	chessWait[11].x = -6.39; chessWait[11].y = 6.85; chessWait[11].z = -3.80;
	chessWait[12].x = -6.05; chessWait[12].y = 5.88; chessWait[12].z = -3.80;
	chessWait[4].x = -6.360; chessWait[4].y = 4.950; chessWait[4].z = -4.100; 
	chessWait[31].x = -6.37; chessWait[31].y = 3.93; chessWait[31].z = -3.60;
	chessWait[30].x = -6.03; chessWait[30].y = 2.95; chessWait[30].z = -3.70; 
	chessWait[29].x = -6.35; chessWait[29].y = 2.00; chessWait[29].z = -3.40;
	chessWait[26].x = -5.82; chessWait[26].y = 1.13; chessWait[26].z = -3.10;
	//黑区编
	chessWait[0].x = 5.170; chessWait[0].y = 8.370; chessWait[0].z = -2.800; 
	chessWait[1].x = 5.200; chessWait[1].y = 7.090; chessWait[1].z = -3.500; 
	chessWait[2].x = 5.190; chessWait[2].y = 6.000; chessWait[2].z = -3.500; 
	chessWait[3].x = 5.190; chessWait[3].y = 4.920; chessWait[3].z = -3.500;
	chessWait[21].x = 5.19; chessWait[21].y = 3.88; chessWait[21].z = -3.00;
	chessWait[22].x = 5.19; chessWait[22].y = 2.79; chessWait[22].z = -2.80;
	chessWait[23].x = 5.19; chessWait[23].y = 1.72; chessWait[23].z = -2.80;
	chessWait[24].x = 5.19; chessWait[24].y = 0.47; chessWait[24].z = -2.80; 
	chessWait[25].x = 5.88; chessWait[25].y = 1.10; chessWait[25].z = -2.80;
	chessWait[27].x = 6.39; chessWait[27].y = 2.01; chessWait[27].z = -3.00; 
	chessWait[28].x = 6.09; chessWait[28].y = 2.95; chessWait[28].z = -3.50;
	chessWait[20].x = 6.42; chessWait[20].y = 3.87; chessWait[20].z = -3.60;
	chessWait[15].x = 6.43; chessWait[15].y = 4.93; chessWait[15].z = -3.80; 
	chessWait[14].x = 6.09; chessWait[14].y = 5.87; chessWait[14].z = -4.00;
	chessWait[13].x = 6.42; chessWait[13].y = 6.86; chessWait[13].z = -4.10;
	chessWait[10].x = 5.91; chessWait[10].y = 7.75; chessWait[10].z = -3.50; 
	
	//读入棋盘坐标信息
    fp = fopen("./chess_game/chessBoard.txt", "r");
	while(!feof(fp)) {
		if(EOF == fscanf(fp, "(%d %d %f %f %f)\n", &r, &c, &x, &y, &z)) {
			printf("double_4-axis::initial_sys: failed\n");
			return -1;
		}
		chessBoard[r][c].x = x;
		chessBoard[r][c].y = y;
		chessBoard[r][c].z = z;
	}
   	
    return 1;
}

//回到默认状态
int back_default_pos()
{
	float obj_xyz[3] = {-4.0, -2.0, 62.70};
	
	//计算运动时间
	//t = cal_move_time(obj_xyz);
	move_to_xyz_t1(obj_xyz, 4.0);
	
	//current_xyz[0] = -4.0; 
	//current_xyz[1] = -2.0;
	
	return 1;
}

//当舵机到达目标刻度的某一范围即可停止等待操作
void wait_move_stop_exten()
{
	wait_for_one_joint_exten(1);
	wait_for_one_joint_exten(2);
	wait_for_one_joint_exten(3);
}
