#include "dmath.h"
#include "action.h"
#include "joint.h"
#include "algorithm.h"
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
static struct park_joint chessWait[32];      //棋子停放列表
static struct chess_pos current_pos[32];   //记录当前棋子
static struct park_joint chessBoard[10][9];  //10行9列
static int get_chess_id[10][9];				 //由坐标获得棋子编号
static float current_xyz[2] = {0, 0};        //存储当前位置
char numPiece = '0';    //棋盘编号
char flag_status = 's'; //棋盘状态，停止状态

//全局函数
int initial_sys(int device, int baudNum);//系统初始化函数
float cal_move_time(float obj_xyz[]);    //由目标点计算出运动时间
void wait_move_stop();                   //等待机械臂完全停止
void wait_move_stop_exten();             //等待机械臂到达目标位置
void r_coor_trans(int r, int c, int *row, int *col);//棋盘坐标转化为指令坐标
int back_default_pos();                             //回到默认状态
int move_chess_rest(int r, int c, int chessNum);    //移子到休息区
int move_chess(int r1, int c1, int r2, int c2);     //走子
int eat_chess(int r1, int c1, int r2, int c2, int chessNum); //吃子
int set_piece(char pieceNum);                       //设置棋盘
char cal_clear_piece_num(char pieceNum);            //由棋盘编号计算出清盘编号
int clear_chess(char pieceFlag);                    //清理棋盘
int win(char pieceNum);                             //结束
int check_moveNum(int chess_id);                    //检测棋子走子次数
int fetch_chess(int chess_id, int r, int c);		//从棋子待放区取子

void receive_instructor(unsigned char instruct[]); //获取指令
int execute_instructor(unsigned char instruct_char[]);//解析并执行指令
void finish_instructor(); //发送完成信号

int main()
{
	unsigned char packet[20] = {0, }; //接收指令和发送指令
	
    //初始化
	if(1 != initial_sys(5, 1200)) {
		printf("error-->double_4-axis::main:initial_sys() failed\n");
		return -1;
	}
	else{
		printf("sys init success...\n");
		back_default_pos();
		//getchar();
	}
    
    do{
    	receive_instructor(packet); //接收指令

		execute_instructor(packet); //解析并执行指令
		
		finish_instructor(); //发送完成信号
        
    }while(1);
    
    return 1;
}

//获取指令
void receive_instructor(unsigned char instruct[])
{
	int flag;

	while(1) {
        //1.获取字符
        flag = receiveMessage(instruct, 20);
		if(-1 == flag) {
			printf("receiving data failed!\r"); fflush(stdout);
		}
		else if(0 == flag) {
			printf("continue receiving data ...\r"); fflush(stdout);
		}
		else if(flag > 0) {
			printf("print data ..."); fflush(stdout);
			if(flag >= 20)
				instruct[19] = '\0';
			else 
				instruct[flag] = '\0';
			printf("the receiving data (%s)\n", instruct); fflush(stdout);
			flag_status = 's'; //设置棋盘状态
			break;
		}
		else {
		}
	}
}

//解析并执行指令
int execute_instructor(unsigned char instruct_char[])
{
	char temp[20] = {0, };
	int info, r1, c1, r2, c2;
	
	//判断是否摆棋
	if('0' <= instruct_char[1] && instruct_char[1] <= '9') {
        numPiece = instruct_char[1]; //设置棋盘编号
        if(1 != set_piece(numPiece)) {
        	printf("double_4-axis::main:set_piece() failed\n");
        	return -1;
        }
        delay_us(1000000); //等待1s后，对弈开始
        flag_status = 'r';
        return 1;
    }
	else if(0 == strcmp("over", (char *)instruct_char)) {
	//判断是否清理棋盘
		delay_us(1000000);
		flag_status = 'r';
		//接收到over信息发送ok
		finish_instructor();
		win(numPiece);  //清理棋盘
		flag_status = 'r';
		return 1;
		
	}
	else if('R' == instruct_char[0]) {
		//取数据
        strcpy(temp, (char *)&instruct_char[2]);
        info = atoi(temp);
        r1 = info / 100000; info %= 100000;
        c1 = info / 10000;  info %= 10000;
        r2 = info / 1000;   info %= 1000;
        c2 = info / 100;    info %= 100;
        
        flag_status = 'r';
        
		//判断是否走子
		if('F' == instruct_char[1]){
			//发生走子行为
			if(1 != move_chess(r1, c1, r2, c2)) {
				printf("double_4-axis::main:move_chess() failed\n");
				return -1;
			}
        }
        else if('T' == instruct_char[1]){
            //判断是否吃子
            if(1 != eat_chess(r1, c1, r2, c2, info)) {
                printf("double_4-axis::main:eat_chess() failed\n");
                return -1;
            }
        }
        else{
        	flag_status = 's';
        }
	}
	else {
		flag_status = 's';
	}
		
	return 1;
}

//发送完成信号
void finish_instructor()
{
	int flag;
	unsigned char return_packet[4] = "okk";
		
	while('r' == flag_status) {
        flag = sendMessage(return_packet, 3);
		if(flag != 3) {
			printf("failed send data\r"); fflush(stdout);
		}
		else {
			printf("success send data:%s\n", return_packet); fflush(stdout);
			flag_status = 's';
			back_default_pos();
			break;
		}	
     }
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
		//打开成功， 则关闭气泵
		close_beng();
	}

    //打开控制机械臂系统，并初始化
    if(1 != initail_system()) {
    	printf("error-->double_4-axis::initial_sys:initail_system\n");
    	return -1;
    }
    
    chessWait[0].x = -5.150; chessWait[0].y = 0.550; chessWait[0].z = -3.300;
	chessWait[1].x = -5.120; chessWait[1].y = 1.850; chessWait[1].z = -3.300;
	chessWait[2].x = -5.120; chessWait[2].y = 2.900; chessWait[2].z = -3.300;
	chessWait[3].x = -5.120; chessWait[3].y = 3.980; chessWait[3].z = -3.300; 
	chessWait[21].x = -5.12; chessWait[21].y = 4.98; chessWait[21].z = -3.30; 
	chessWait[22].x = -5.12; chessWait[22].y = 6.06; chessWait[22].z = -3.20; 
	chessWait[23].x = -5.12; chessWait[23].y = 7.15; chessWait[23].z = -3.90; 
	chessWait[24].x = -5.12; chessWait[24].y = 8.47; chessWait[24].z = -2.50;  
	chessWait[25].x = -5.84; chessWait[25].y = 7.80; chessWait[25].z = -2.80; 
	chessWait[27].x = -6.32; chessWait[27].y = 6.94; chessWait[27].z = -2.70; 
	chessWait[28].x = -6.00; chessWait[28].y = 5.96; chessWait[28].z = -2.90;
	chessWait[20].x = -6.35; chessWait[20].y = 5.00; chessWait[20].z = -3.20;
	chessWait[15].x = -6.35; chessWait[15].y = 4.00; chessWait[15].z = -3.20;
	chessWait[14].x = -6.02; chessWait[14].y = 3.03; chessWait[14].z = -3.20;
	chessWait[13].x = -6.32; chessWait[13].y = 2.07; chessWait[13].z = -3.30;
	chessWait[10].x = -5.83; chessWait[10].y = 1.20; chessWait[10].z = -3.30;
    
    chessWait[16].x = 5.25; chessWait[16].y = 8.42; chessWait[16].z = -2.50;
    chessWait[17].x = 5.22; chessWait[17].y = 7.11; chessWait[17].z = -3.50;
    chessWait[18].x = 5.21; chessWait[18].y = 6.04; chessWait[18].z = -3.60;
    chessWait[19].x = 5.20; chessWait[19].y = 4.97; chessWait[19].z = -3.60;
    chessWait[5].x = 5.190; chessWait[5].y = 3.950; chessWait[5].z = -3.700;
    chessWait[6].x = 5.180; chessWait[6].y = 2.870; chessWait[6].z = -3.700;
    chessWait[7].x = 5.180; chessWait[7].y = 1.820; chessWait[7].z = -3.600;
    chessWait[8].x = 5.170; chessWait[8].y = 0.540; chessWait[8].z = -3.700;
    chessWait[9].x = 5.890; chessWait[9].y = 1.170; chessWait[9].z = -3.700;
    chessWait[11].x = 6.39; chessWait[11].y = 2.03; chessWait[11].z = -3.90;
    chessWait[12].x = 6.07; chessWait[12].y = 2.99; chessWait[12].z = -3.80;
    chessWait[4].x = 6.420; chessWait[4].y = 3.950; chessWait[4].z = -3.800;
    chessWait[31].x = 6.44; chessWait[31].y = 4.96; chessWait[31].z = -3.80;
    chessWait[30].x = 6.12; chessWait[30].y = 5.93; chessWait[30].z = -3.80;
    chessWait[29].x = 6.46; chessWait[29].y = 6.89; chessWait[29].z = -3.70;
    chessWait[26].x = 5.95; chessWait[26].y = 7.77; chessWait[26].z = -3.40;
    
    //读入棋盘坐标信息
	fp = fopen("./chess_game/chessBoard", "r");
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

//计算出运动时间
float cal_move_time(float obj_xyz[])
{
	float distance;
	
	distance = pow((obj_xyz[0] - current_xyz[0]), 2);
	distance += pow((obj_xyz[1] - current_xyz[1]), 2);
	
	distance = sqrt(distance);
	
	if(distance <= 2.84)
		return 0.50;
	else if(distance <= 4.68)
		return 1.00;
	else if(distance <= 6.52)
		return 0.90;
	else if(distance <= 8.36)
		return 1.20;
	else if(distance <= 10.2)
		return 1.50;
	else if(distance <= 12.04)
		return 1.80;
	else
		return 2.10;
}

//由输入棋盘数据计算输出棋盘数据
char cal_clear_piece_num(char pieceNum)
{
	if('0' == pieceNum)
		return 'o';
	else if('1' == pieceNum)
		return 'a';
	else if('2' == pieceNum)
		return 'b';
	else if('3' == pieceNum)
		return 'c';
	else if('4' == pieceNum)
		return 'd';
	else 
		return 'd';	
}

//等待目标位置完全停止
void wait_move_stop()
{
	wait_for_one_joint(1);
	wait_for_one_joint(2);
	wait_for_one_joint(3);
	delay_us(500000);
}

//当舵机到达目标刻度的某一范围即可停止等待操作
void wait_move_stop_exten()
{
	wait_for_one_joint_exten(1);
    wait_for_one_joint_exten(2);
    wait_for_one_joint_exten(3);
}

//坐标转换：棋盘坐标转化为指令坐标
void r_coor_trans(int r, int c, int *row, int *col)
{
	//计算列号
	*col = c + 4;
	
	//计算行号
	*row = 9 - r;
}

//回到默认状态
int back_default_pos()
{
	float obj_xyz[3] = {4.0, -1.8, 62.70}, t;
	
	t = cal_move_time(obj_xyz);
	move_to_xyz_t1(obj_xyz, t+0.5);
	
	current_xyz[0] = 4.00;
	current_xyz[1] = -1.80;
	
	return 1;
}

//移动子到存放区休息, 然后更新棋盘信息
//(r, c)指令坐标
int move_chess_rest(int r, int c, int chessNum)
{
	float obj_xyz[3], t;
	
	//移动棋子到存放区
	//棋子坐标
	obj_xyz[0] = chessBoard[r][c].x;
	obj_xyz[1] = chessBoard[r][c].y;
	obj_xyz[2] = 37.72; //移动棋子上方
	t = cal_move_time(obj_xyz); //计算移动时间
	move_to_xyz_t1(obj_xyz, t); 
	wait_move_stop_exten();
	current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1];
	
	open_beng();		//打开气泵
	
	//向下(保持稳定)
	obj_xyz[2] = chessBoard[r][c].z + 1.0; //向下运动
	move_to_xyz_t1(obj_xyz, 0.8);
	wait_move_stop();
	
	//向下取子
	obj_xyz[2] = chessBoard[r][c].z - 2.0;//取子
	move_to_xyz_t1(obj_xyz, 0.2);
	wait_move_stop();
	
	//回向运动
	obj_xyz[2] = 37.72; //向上回运动
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	
	//把棋子放入待放区
	obj_xyz[0] = chessWait[chessNum - 1].x;
	obj_xyz[1] = chessWait[chessNum - 1].y;
	obj_xyz[2] = 37.72;   //运动到棋子待放区上方
	t = cal_move_time(obj_xyz);
	move_to_xyz_t1(obj_xyz, t);
	wait_move_stop_exten();
	current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1];
	
	obj_xyz[2] = chessWait[chessNum - 1].z + 4.5; //向下运动
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	
	close_beng();        //关闭气泵
	
	obj_xyz[2] = 37.72;  //向上回运动
	move_to_xyz_t1(obj_xyz, 1.0);
	wait_move_stop_exten();
	
	return 1;
}

//走子 = 走子
//(r1, c1, r2, c2):指令坐标
int move_chess(int r1, int c1, int r2, int c2)
{
	float obj_xyz[3], t;
	int chessNum; //棋子编号
	
	chessNum = get_chess_id[r1][c1];
	//判断是否需要重新校准棋子
	if(1 == check_moveNum(chessNum)) {
		//1.0 需要重新校准棋子
		//1.1 把棋子放会棋子待放区
		move_chess_rest(r1, c1, chessNum);
		
		//1.2 取子到目标区域
		fetch_chess(chessNum, r2, c2);
		
		//1.3 更新坐标信息
		current_pos[chessNum - 1].moveNum = 0;
	}
	else {
		//2.0 不需要重新校准棋子
		//2.1 启动棋子上方
		obj_xyz[0] = chessBoard[r1][c1].x;
		obj_xyz[1] = chessBoard[r1][c1].y;
		obj_xyz[2] = 37.72;    //运动到棋子上方
		t = cal_move_time(obj_xyz); //计算运动时间俺
		move_to_xyz_t1(obj_xyz, t);
		wait_move_stop_exten(); 
		current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1];
		
		//2.2 打开气泵
		open_beng();

		//2.3 向下运动(保持稳定)
		obj_xyz[2] = chessBoard[r1][c1].z + 1.0;    //向下运动
		move_to_xyz_t1(obj_xyz, 0.8);
		wait_move_stop_exten();

		//2.4 向下运动(取子)
		obj_xyz[2] = chessBoard[r1][c1].z - 2.0;    //向下取子
		move_to_xyz_t1(obj_xyz, 0.2);
		wait_move_stop();
		
		//2.5 回向运动
		obj_xyz[2] = 37.72;   //向上回运动
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();


		//3.0 把棋子移到目标区域
		obj_xyz[0] = chessBoard[r2][c2].x;
		obj_xyz[1] = chessBoard[r2][c2].y;
		obj_xyz[2] = 37.72;   //运动到目标区上方
		t = cal_move_time(obj_xyz);
		move_to_xyz_t1(obj_xyz, t);
		wait_move_stop_exten();
		current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1]; 

		//3.1 向下运动
		obj_xyz[2] = chessBoard[r2][c2].z - 4.1; //3.7
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop();

		//3.2 关闭气泵
		close_beng();

		//3.3 回向运动
		obj_xyz[2] = 37.72;   //向上回运动 
		move_to_xyz_t1(obj_xyz, 1.0);
		wait_move_stop_exten();
		
		//4.0 更新坐标信息
		current_pos[chessNum - 1].moveNum ++;
	}

	get_chess_id[r1][c1] = -1;
	get_chess_id[r2][c2] = chessNum;
	current_pos[chessNum - 1].r = r2;
	current_pos[chessNum - 1].c = c2;
	
	printf("\nchess(%d) coor(%d %d) moveNum(%d)\n\n", get_chess_id[r2][c2], current_pos[chessNum - 1].r, current_pos[chessNum - 1].c, current_pos[chessNum - 1].moveNum);

	return 1;
}

//吃子 = 移子 + 走子
int eat_chess(int r1, int c1, int r2, int c2, int chessNum)
{
	//移动棋子
	if(1 != move_chess_rest(r2, c2, chessNum)) {
		printf("double_4-axis::eat_chess:move_chess_rest:failed\n");
		return -1;
	}
	else {
		//更新坐标信息
		
	}
	
	//走子
	if(1 != move_chess(r1, c1, r2, c2)) {
		printf("double_4-axis::eat_chess:move_chess(r1, c1, r2, c2)\n");
		return -1;
	}
	
	return 1;
}

//设置残局
int set_piece(char pieceNum)
{
	int r, c, num;
	char path[20] = {0, };
	float obj_xyz[3], t;
	
	//计算棋谱路径
	sprintf(path, "./chess_game/%c.txt", pieceNum);
	
	//读取棋谱数据
	FILE *fp = fopen(path, "r");
	while(!feof(fp)) {
		if(EOF == fscanf(fp, "(%d %d)-->%d\n", &r, &c, &num)) {
			printf("double_4-axis::set_piece:chess_game failed\n");
			return -1;
		}
		
		//从棋子待放区取棋子
		obj_xyz[0] = chessWait[num - 1].x;
		obj_xyz[1] = chessWait[num - 1].y;
		obj_xyz[2] = 37.72;
		t = cal_move_time(obj_xyz); //计算时间
		move_to_xyz_t1(obj_xyz, t); //设置目标位置(包含动作完成等待函数)
		wait_move_stop_exten();
		current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1];
		
		//向下运动(保持稳定)
		obj_xyz[2] = chessWait[num - 1].z + 1.0;
		move_to_xyz_t1(obj_xyz, 0.8); //设置目标位置(包含动作完成等待函数)
		wait_move_stop();
		//向下运动(取子)
		obj_xyz[2] = chessWait[num - 1].z - 2.0;
		move_to_xyz_t1(obj_xyz, 0.2); //设置目标位置(包含动作完成等待函数)
		wait_move_stop();
		
		open_beng();
		
		//回向运动
		obj_xyz[2] = 37.72;
		move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
		wait_move_stop_exten();
		
		//摆子到棋盘位置
		obj_xyz[0] = chessBoard[r][c].x;
		obj_xyz[1] = chessBoard[r][c].y;
		obj_xyz[2] = 37.72;
		t = cal_move_time(obj_xyz);
		move_to_xyz_t1(obj_xyz, t); //设置目标位置(包含动作完成等待函数)
		wait_move_stop_exten();
		current_xyz[0] = obj_xyz[0];current_xyz[1] = obj_xyz[1];
		
		//下降
		obj_xyz[2] = chessBoard[r][c].z - 4.1; //
		move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
		wait_move_stop();
		
		close_beng();
		
		obj_xyz[2] = 37.72;
		move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
		wait_move_stop_exten();
		
		//记录棋子当前位置
		current_pos[num - 1].r = r;
		current_pos[num - 1].c = c;
		current_pos[num - 1].moveNum = 0;
		
		//记录棋子编号
		get_chess_id[r][c] = num;
	}
	fclose(fp);
	
	//初始化current_pos和get_chess_i
	sprintf(path, "./chess_game/s%c.txt", pieceNum);
	fp = fopen(path, "r");
	while(!feof(fp)) {
		if(EOF == fscanf(fp, "(%d %d)-->%d\n", &r, &c, &num)) {
			printf("double_4-axis::set_piece:init current_pos and get_chess_id failed\n");
			return -1;
		}
		get_chess_id[r][c] = num;
		current_pos[num-1].r = r;
		current_pos[num-1].c = c;
		current_pos[num-1].moveNum = 0;
	}
	fclose(fp);
	
	return 1;
}

//收拾桌面棋子
int clear_chess(char pieceFlag)
{
	int r, c, num;
	char path[20] = {0, };
	
	//计算棋谱路径
	sprintf(path, "./chess_game/%c.txt", pieceFlag);
	
	//读取棋谱数据
	FILE *fp = fopen(path, "r");
	while(!feof(fp)) {
		if(EOF == fscanf(fp, "(%d %d)-->%d\n", &r, &c, &num)) {
			printf("double_4-axis::set_piece:chess_game failed\n");
			return -1;
		}
		
		//把棋子摆到棋子休息区
		move_chess_rest(r, c, num);
	}
	
	return 1;
}

//胜利
int win(char pieceNum)
{
	char clearNum;
	
	//计算清盘编号
	clearNum = 	cal_clear_piece_num(pieceNum); 
	
	//清盘
	clear_chess(clearNum);
	
	return 1;
}

//检测棋子走子编号
int check_moveNum(int chess_id)
{
	int r, moveNum;
	
	r = current_pos[chess_id - 1].r;
	moveNum = current_pos[chess_id].moveNum;
		
	if((r >= 5) && (moveNum >= 4))
		return 1;
	if((r <= 4) && (moveNum >= 8))
		return 1;
	
	return 0;
}

//重新取子
int fetch_chess(int chess_id, int r, int c)
{
	float obj_xyz[3], t;
	
	//运动到棋子待放区
	//1.0 获取棋子在棋子待放区的坐标并运动至此
	obj_xyz[0] = chessWait[chess_id - 1].x;
	obj_xyz[1] = chessWait[chess_id - 1].y;
	obj_xyz[2] = 37.72;
	t = cal_move_time(obj_xyz); //计算时间
	move_to_xyz_t1(obj_xyz, t); //设置目标位置(包含动作完成等待函数)
	wait_move_stop_exten();
	current_xyz[0] = obj_xyz[0]; current_xyz[1] = obj_xyz[1];
		
	//1.1 向下运动(保持稳定)
	obj_xyz[2] = chessWait[chess_id - 1].z + 1.0;
	move_to_xyz_t1(obj_xyz, 0.8); //设置目标位置(包含动作完成等待函数)
	wait_move_stop();
	//1.2 向下运动(取子)
	obj_xyz[2] = chessWait[chess_id - 1].z - 2.0;
	move_to_xyz_t1(obj_xyz, 0.2); //设置目标位置(包含动作完成等待函数)
	wait_move_stop();
	
	//1.3 打开气泵
	open_beng();
		
	//1.4 回向运动
	obj_xyz[2] = 37.72;
	move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
	wait_move_stop_exten();
		
	//2.0 获得目标位置坐标并运动至此
	obj_xyz[0] = chessBoard[r][c].x;
	obj_xyz[1] = chessBoard[r][c].y;
	obj_xyz[2] = 37.72;
	t = cal_move_time(obj_xyz);
	move_to_xyz_t1(obj_xyz, t); //设置目标位置(包含动作完成等待函数)
	wait_move_stop_exten();
	current_xyz[0] = obj_xyz[0];current_xyz[1] = obj_xyz[1];
		
	//2.1 下降放棋子
	obj_xyz[2] = chessBoard[r][c].z - 4.1;
	move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
	wait_move_stop();
	
	//2.2 关闭气泵
	close_beng();
	
	//2.3 向上回向运动
	obj_xyz[2] = 37.72;
	move_to_xyz_t1(obj_xyz, 1.0); //设置目标位置(包含动作完成等待函数)
	wait_move_stop_exten();
	
	return 1;
}		

