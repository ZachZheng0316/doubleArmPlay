#include "playChess.h"
#include "action.h"
#include "beng.h"
#include <stdio.h>


static struct XYZ chessWait[32];
static struct XYZ chess_init[32];

//初始化棋子待放区
void init_chess_wait()
{
    //棋子待放区坐标
    chessWait[0].x = -5.20;  chessWait[0].y = 0.50;  chessWait[0].z = 28.45;
	chessWait[1].x = -5.20;  chessWait[1].y = 1.81;  chessWait[1].z = 28.45;
	chessWait[2].x = -5.20;  chessWait[2].y = 2.86;  chessWait[2].z = 28.45;
	chessWait[3].x = -5.20;  chessWait[3].y = 3.94;  chessWait[3].z = 28.45;
	chessWait[4].x = -5.20;  chessWait[4].y = 4.91;  chessWait[4].z = 28.45;
	chessWait[5].x = -5.23;  chessWait[5].y = 5.96;  chessWait[5].z = 28.45;
	chessWait[6].x = -5.23;  chessWait[6].y = 7.03;  chessWait[6].z = 28.45;
	chessWait[7].x = -5.23;  chessWait[7].y = 8.35;  chessWait[7].z = 28.45;
	chessWait[8].x = -5.89;  chessWait[8].y = 7.71;  chessWait[8].z = 28.45;
	chessWait[9].x = -6.45;  chessWait[9].y = 6.92;  chessWait[9].z = 28.45;
	chessWait[10].x = -6.05; chessWait[10].y = 5.91; chessWait[10].z = 28.45;
	chessWait[11].x = -6.43; chessWait[11].y = 4.95; chessWait[11].z = 28.45;
	chessWait[12].x = -6.37; chessWait[12].y = 3.95; chessWait[12].z = 28.45;
	chessWait[13].x = -6.07; chessWait[13].y = 3.00; chessWait[13].z = 28.45;
	chessWait[14].x = -6.36; chessWait[14].y = 2.08; chessWait[14].z = 28.45;
	chessWait[15].x = -5.88; chessWait[15].y = 1.16; chessWait[15].z = 28.45;
	//黑区编
	chessWait[16].x = 5.19;  chessWait[16].y = 0.60; chessWait[16].z = 28.45;
	chessWait[17].x = 5.15;  chessWait[17].y = 1.86; chessWait[17].z = 28.45;
	chessWait[18].x = 5.14;  chessWait[18].y = 2.94; chessWait[18].z = 28.45;
	chessWait[19].x = 5.14;  chessWait[19].y = 4.00; chessWait[19].z = 28.45;
	chessWait[20].x = 5.14;  chessWait[20].y = 5.03; chessWait[20].z = 28.45;
	chessWait[21].x = 5.14;  chessWait[21].y = 6.08; chessWait[21].z = 28.45;
	chessWait[22].x = 5.14;  chessWait[22].y = 7.17; chessWait[22].z = 28.45;
	chessWait[23].x = 5.20;  chessWait[23].y = 8.44; chessWait[23].z = 28.45;
	chessWait[24].x = 5.81;  chessWait[24].y = 7.81; chessWait[24].z = 28.45;
	chessWait[25].x = 6.36;  chessWait[25].y = 6.88; chessWait[25].z = 28.45;
	chessWait[26].x = 6.01;  chessWait[26].y = 5.91; chessWait[26].z = 28.45;
	chessWait[27].x = 6.38;  chessWait[27].y = 4.94; chessWait[27].z = 28.45;
	chessWait[28].x = 6.40;  chessWait[28].y = 3.95; chessWait[28].z = 28.45;
	chessWait[29].x = 6.01;  chessWait[29].y = 3.00; chessWait[29].z = 28.45;
	chessWait[30].x = 6.38;  chessWait[30].y = 2.05; chessWait[30].z = 28.45;
	chessWait[31].x = 5.81;  chessWait[31].y = 1.23; chessWait[31].z = 28.45;
}

//初始化棋子初始化位置
void init_chess_pos()
{
    //棋子初始化位置
    chess_init[0].x = -4.0;  chess_init[0].y = 0.0;  chess_init[0].z = 28.45;
    chess_init[1].x = -3.0;  chess_init[1].y = 0.0;  chess_init[1].z = 28.45;
    chess_init[2].x = -2.0;  chess_init[2].y = 0.0;  chess_init[2].z = 28.45;
    chess_init[3].x = -1.0;  chess_init[3].y = 0.0;  chess_init[3].z = 28.45;
    chess_init[4].x = 0.0;   chess_init[4].y = 0.0;  chess_init[4].z = 28.45;
    chess_init[5].x = 1.0;   chess_init[5].y = 0.0;  chess_init[5].z = 28.45;
    chess_init[6].x = 2.0;   chess_init[6].y = 0.0;  chess_init[6].z = 28.45;
    chess_init[7].x = 3.0;   chess_init[7].y = 0.0;  chess_init[7].z = 28.45;
    chess_init[8].x = 4.0;   chess_init[8].y = 0.0;  chess_init[8].z = 28.45;
    chess_init[9].x = -3.0;  chess_init[9].y = 2.0;  chess_init[9].z = 28.45;
    chess_init[10].x = 3.0;  chess_init[10].y = 2.0; chess_init[10].z = 28.45;
    chess_init[11].x = -4.0; chess_init[11].y = 3.0; chess_init[11].z = 28.45;
    chess_init[12].x = -2.0; chess_init[12].y = 3.0; chess_init[12].z = 28.45;
    chess_init[13].x = 0.0;  chess_init[13].y = 3.0; chess_init[13].z = 28.45;
    chess_init[14].x = 2.0;  chess_init[14].y = 3.0; chess_init[14].z = 28.45;
    chess_init[15].x = 4.0;  chess_init[15].y = 3.0; chess_init[15].z = 28.45;

    chess_init[16].x = 4.0;  chess_init[16].y = 9.0; chess_init[16].z = 28.45;
    chess_init[17].x = 3.0;  chess_init[17].y = 9.0; chess_init[17].z = 28.45;
    chess_init[18].x = 2.0;  chess_init[18].y = 9.0; chess_init[18].z = 28.45;
    chess_init[19].x = 1.0;  chess_init[19].y = 9.0; chess_init[19].z = 28.45;
    chess_init[20].x = 0.0;  chess_init[20].y = 9.0; chess_init[20].z = 28.45;
    chess_init[21].x = -1.0; chess_init[21].y = 9.0; chess_init[21].z = 28.45;
    chess_init[22].x = -2.0; chess_init[22].y = 9.0; chess_init[22].z = 28.45;
    chess_init[23].x = -3.0; chess_init[23].y = 9.0; chess_init[23].z = 28.45;
    chess_init[24].x = -4.0; chess_init[24].y = 9.0; chess_init[24].z = 28.45;
    chess_init[25].x = 3.0;  chess_init[25].y = 7.0; chess_init[25].z = 28.45;
    chess_init[26].x = -3.0; chess_init[26].y = 7.0; chess_init[26].z = 28.45;
    chess_init[27].x = 4.0;  chess_init[27].y = 6.0; chess_init[27].z = 28.45;
    chess_init[28].x = 2.0;  chess_init[28].y = 6.0; chess_init[28].z = 28.45;
    chess_init[29].x = 0.0;  chess_init[29].y = 6.0; chess_init[29].z = 28.45;
    chess_init[30].x = -2.0; chess_init[30].y = 6.0; chess_init[30].z = 28.45;
    chess_init[31].x = -4.0; chess_init[31].y = 6.0; chess_init[31].z = 28.45;
}

void put_chess()
{
    int i;
    float obj_xyz[3], t2 = 1.0;

    init_chess_wait(); //初始化棋子待放区
    init_chess_pos(); //初始化棋子初始化位置

    for(i = 0; i < 32; i++) {
        //1.从棋子待放区取子
            //1.0 计算目标区
            obj_xyz[0] = chessWait[i].x;
            obj_xyz[1] = chessWait[i].y;
            obj_xyz[2] = chessWait[i].z;
            //1.1 运动到棋子上方
//            set_spe(obj_xyz, t1);
//            move_to_xyz(obj_xyz);
            move_to_xyz_t(obj_xyz);
//            getchar();
            //1.2 打开气泵
            open_beng();
            //1.3 向下吸子
            up_down_move_1(-25.0, t2);
//            getchar();
            //1.4 向上运动
            up_down_move_1(25.0, t2);
//            getchar();

        //2.运动到棋子目标区
            //2.0 计算目标区
            obj_xyz[0] = chess_init[i].x;
            obj_xyz[1] = chess_init[i].y;
            obj_xyz[2] = chess_init[i].z;
            //2.2 运动到目标区上方
//            set_spe(obj_xyz, t1);
//            move_to_xyz(obj_xyz);
            move_to_xyz_t(obj_xyz);
//            getchar();
            //2.4 向下运动
            up_down_move_1(-20.0, t2);
//            getchar();
            //2.5 关闭气泵放子
            close_beng();
            //2.6 向上运动
            up_down_move_1(20.0, t2);
//            getchar();
    }

    obj_xyz[0] = 0.0; obj_xyz[1] = 4.0; obj_xyz[2] = 28.45;
    move_to_xyz_t(obj_xyz);

    close_beng();
}
