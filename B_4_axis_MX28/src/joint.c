#include "MX28AT.h"
#include "dmath.h"
#include "joint.h"
#include <stdio.h>

#define Joint_Unit (3) //关节组的最大个数

static int joint_num = 3; //关节个数默认为3
static int joint_unit[Joint_Unit] = {1, 2, 3}; //关节组

//设置舵机个数
void set_joint_num(int num)
{
    joint_num = num;
}

//设置舵机组
void set_joint_unit(int joint_unit_temp[])
{
    int i;
    for(i = 0; i < Joint_Unit; i++)
        joint_unit[i] = joint_unit_temp[i];
}

//设置单个舵机运动速度
//Succeeded, return 1; failed, return 0;
int set_one_joint_angle_spe(int joint_id, float angle_spe)
{
    float rad_spe;
    int spe_k;

    rad_spe = PIFromAngle(angle_spe);//把关节角速度转化为关节弧速度
    spe_k = kFromRadianSpeed((unsigned char)joint_id, rad_spe);//把关节弧速度转化为对应舵机速度刻度
    //设置相应舵机速度
    if(-1 == set_one_servo_word(joint_id, Moving_Speed, spe_k)) {
        printf("joint::set_one_joint_angle_spe:failed\n");
        return -1;
    }

    return 1;
}

//设置单个舵机目标角度
int set_one_joint_goal_angle(int joint_id, float goal_angle)
{
    int goal_pos;
     //已知关节当前角度转换为舵机当前刻度
     goal_pos = positionKFromAngle((unsigned char)joint_id, goal_angle);

     //设置目标刻度
     if(-1 == set_one_servo_word(joint_id, Goal_Position, goal_pos)) {
         printf("joint::set_one_joint_goal_angle:failed\n");
         return -1;
     }

     return 1;
}

//设置多个舵机的速度
int set_many_joint_angle_spe(float angle_spe[])
{
    int spe_k[Joint_Unit], i;
    float rad_spe;

    //把关节角速度转化为舵机速度刻度
    for(i = 0; i < joint_num; i++) {
        rad_spe = PIFromAngle(angle_spe[i]);//把关节角速度转化为关节弧速度
        spe_k[i] = kFromRadianSpeed((unsigned char)joint_unit[i], rad_spe);//把关节弧速度转化为对应舵机速度刻度
    }

    //设置速度
    set_servo_num(joint_num); //设置舵机个数
    set_servo_unit(joint_unit); //设置舵机组
    if(-1 == set_many_servo_word(Moving_Speed, spe_k)){
        printf("joint::set_many_joint_angle_spe:failed\n");
        return -1;
    }
//	printf("joint::set_many_angle_spe:spe_k(%d, %d, %d)\n", spe_k[0], spe_k[1], spe_k[2]);

    return 1;
}

//设置多个舵机的目标刻度
int set_many_joint_goal_angle(float goal_angle[])
{
    int goal_pos[Joint_Unit], i;

    //获取目标刻度
    for(i = 0; i < joint_num; i++) {
        goal_pos[i] = positionKFromAngle((unsigned char)joint_unit[i], goal_angle[i]);
    }

    //设置目标位置
    set_servo_num(joint_num); //设置舵机个数
    set_servo_unit(joint_unit); //设置舵机组
    if(-1 == set_many_servo_word(Goal_Position, goal_pos)){
        printf("joint::set_many_joint_goal_angle:failed\n");
        return -1;
    }

    return 1;
}

//获取单个关节目标速度
float get_one_joint_angle_spe(int joint_id)
{
    return (float)joint_id;
}

//获取单个关节目标角度
float get_one_joint_goal_angle(int joint_id)
{
    int servo_goal_k;
    float joint_goal_angle;
    //获取关节的目标刻度
    servo_goal_k = get_one_servo_word(joint_id, Present_Position);
    //把关节目标刻度转化为关节角度
    joint_goal_angle = angleFromPositionK((unsigned char)joint_id, servo_goal_k);

    return joint_goal_angle;
}

//等待单个关节停止
int wait_for_one_joint(int joint_id)
{
    wait_for_one_servo(joint_id); //等待1个舵机运动停止
    return 1;
}

//等待某个关节角度到达一定范围
int wait_for_one_joint_exten(int joint_id)
{
    wait_for_one_servo_exten(joint_id, 5);

    return 1;
}

//控制舵机的demo
void joint_control_demo()
{
    char choice;
    int joint_id[3];
    float value1[3], value2[3];
    printf("欢迎进入控制关节的demo...\n");fflush(stdout);
    while(1) {
        printf("请选择控制模式:q:退出; s:单个关节；m:控制多个关节\n");
        fflush(stdout);getchar();fflush(stdin);
        if(0 == scanf("%c", &choice)) return ;
        if('q' == choice) {
            printf("正在退出关节控制模式...\n");fflush(stdout);
            break;
        }
        else if('s' == choice) {
            printf("欢迎进入单关节控制模式\n");
            printf("请输入关节ID:");fflush(stdout);
            if(0 == scanf("%d", &joint_id[0])) return;fflush(stdin);
            value1[0] = get_one_joint_goal_angle(joint_id[0]);
            printf("%d舵机的当前位置%f\n", joint_id[0], value1[0]);
            printf("请输入关节目标角度和运动角速度:");fflush(stdout);
            if(0 == scanf("%f %f", &value1[0], &value2[0])) return;
            set_one_joint_angle_spe(joint_id[0], value2[0]);//设置速度
            set_one_joint_goal_angle(joint_id[0], value1[0]); //设置目标位置
            wait_for_one_joint(joint_id[0]); //等待关节运动结束
        }
        else if('m' == choice) {
            printf("欢迎进入多关节控制模式\n");
            printf("请输入关节id1, id2, id3:");fflush(stdout);
            if(0 ==scanf("%d %d %d", &joint_id[0], &joint_id[1], &joint_id[2])) return;fflush(stdin);
            //获得当前位置
            value1[0] = get_one_joint_goal_angle(joint_id[0]); //
            value1[1] = get_one_joint_goal_angle(joint_id[1]);
            value1[2] = get_one_joint_goal_angle(joint_id[2]);
            printf("%d关节当前位置%f\n", joint_id[0], value1[0]);
            printf("%d关节当前位置%f\n", joint_id[1], value1[1]);
            printf("%d关节当前位置%f\n", joint_id[2], value1[2]);
            printf("请输入舵机速度sp1, sp2, sp3:");fflush(stdout);
            if(0 == scanf("%f %f %f", &value1[0], &value1[1], &value1[2])) return;fflush(stdin);
            set_servo_num(3);//设置关节个数
            set_servo_unit(joint_id);//设置关节组
            set_many_joint_angle_spe(value1);//设置速度
            printf("请输入关节目标角度ga1, ga2, ga3:");fflush(stdout);
            if(0 == scanf("%f %f %f", &value2[0], &value2[1], &value2[2])) return;fflush(stdin);
            set_many_joint_goal_angle(value2);//设置目标位置
            wait_for_one_joint(joint_id[0]); //等待关节运动结束
            wait_for_one_joint(joint_id[1]); //等待关节运动结束
            wait_for_one_joint(joint_id[2]); //等待关节运动结束
        }
        else {
        }
    }
}
