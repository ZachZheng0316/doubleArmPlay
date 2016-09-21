#include <stdio.h>
#include "MX28AT.h"
#include "joint.h"
#include "algorithm.h"
#include "action.h"
#include "playChess.h"
#include "demo.h"

//delta控制函数
//包含对delta舵机，关节和执行端简单的控制
//执行成功，返回1; 执行失败, 返回-1;
int delta_control()
{
    int control_flag;

    //系统初始化
    if(-1 == initail_system()) {
        printf("demo::delta_control:failed to init\n");
        return -1;
    }

    //控制选项
    while(1) {
        printf("please select the choice you want:\n");
        printf("1, servo control; 2, joint control; 3, xyz control; 4, playChess; 5, end_control:");
        fflush(stdout);fflush(stdin);
        if(0 == scanf("%d", &control_flag)) return 0;
        //control_flag = 'd';
        if(1 == control_flag) {
            servo_control_demo();
            //return 1;
        }

        if(2 == control_flag) {
            joint_control_demo();
            //return 1;
        }

        if(3 == control_flag) {
            xyz_control_demo();
            //return 1;
        }

        if(4 == control_flag) {
            put_chess();
            //return 1;
        }

        if(5 == control_flag) {
            break;
        }
    }
        return 1;
}
