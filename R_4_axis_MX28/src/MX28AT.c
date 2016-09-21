#include "MX28AT.h"
#include "dynamixel.h"
#include "dmath.h"
#include "dxl_hal.h"
#include <math.h>
#include <stdio.h>
#include <unistd.h>


#define Servo_Unit (3) //舵机组的最大个数

static int servo_num = 3; //舵机个数默认为1
static int servo_unit[Servo_Unit] = {1, 2, 3}; //舵机组

//设置舵机个数
void set_servo_num(int num)
{
    servo_num = num;
}

//设置舵机组
void set_servo_unit(int servo_unit_temp[])
{
    int i = 0;
    for (i = 0; i < (int)servo_num; i++)
        servo_unit[i] = servo_unit_temp[i];
}

//设置单个舵机单字节属性
//成功写入，返回1; 写入失败返回-1；
int set_one_servo_byte(int id, int address, int value)
{
    int commStatus, write_num = 20;

    while(write_num-- >= 0) {
        //设置属性值
        dxl_write_byte(id, address, value);
        //判断是否成功写入
        commStatus = dxl_get_result();
        if( commStatus == COMM_RXSUCCESS ) {
            PrintErrorCode();
            return 1;
        }
        else {
            PrintCommStatus(commStatus);
            //return -1;
        }
    }

    return -1;
}

//设置单个舵机多字节属性
//写入成功，返回1; 写入失败， 返回-1
int set_one_servo_word(int id, int address, int value)
{
    int commStatus, write_num = 20;

    while(write_num-- >= 0) {
        dxl_write_word(id, address, value);
        commStatus = dxl_get_result();
        if( commStatus == COMM_RXSUCCESS ) {
            PrintErrorCode();
            return 1;
        }
        else {
            PrintCommStatus(commStatus);
            //return -1;
        }
    }

    return -1;
}

//设置多个舵机单字节属性
//写入成功, 返回1; 写入失败, 返回-1。
int set_many_servo_byte(int address, int value[])
{
    int commStatus, i, write_num = 20;

    while(write_num-- >= 0) {
        // Make syncwrite packet
        dxl_set_txpacket_id(BROADCAST_ID);
        dxl_set_txpacket_instruction(INST_SYNC_WRITE);
        dxl_set_txpacket_parameter(0, address);
        dxl_set_txpacket_parameter(1, 1);
        for( i = 0; i < servo_num; i++ ) {
            dxl_set_txpacket_parameter(2 + 2 * i, servo_unit[i]);
            dxl_set_txpacket_parameter(2 + 2 * i + 1, value[i]);
        }
        dxl_set_txpacket_length((1 + 1) * servo_num + 4);
        dxl_txrx_packet();
        commStatus = dxl_get_result();
        if( commStatus == COMM_RXSUCCESS ) {
            PrintErrorCode();
            return 1;
        }
        else{
            PrintCommStatus(commStatus);
            //return -1;
        }
    }
    return -1;
}

//设置多个舵机多自己属性
//写入成功，返回1; 写入失败, 返回-1.
int set_many_servo_word(int address, int value[])
{
    int commStatus, i, write_num = 20;

    while(write_num-- >= 20) {
        // Make syncwrite packet
        dxl_set_txpacket_id(BROADCAST_ID);
        dxl_set_txpacket_instruction(INST_SYNC_WRITE);
        dxl_set_txpacket_parameter(0, address);
        dxl_set_txpacket_parameter(1, 2);
        for( i = 0; i < servo_num; i++ ) {
            dxl_set_txpacket_parameter(2 + 3 * i, servo_unit[i]);
            dxl_set_txpacket_parameter(2 + 3 * i + 1, dxl_get_lowbyte(value[i]));
            dxl_set_txpacket_parameter(2 + 3 * i + 2, dxl_get_highbyte(value[i]));
        }
        dxl_set_txpacket_length((2 + 1) * servo_num + 4);
        dxl_txrx_packet();
        commStatus = dxl_get_result();
        if( commStatus == COMM_RXSUCCESS ) {
            PrintErrorCode();
            return 1;
        }
        else{
            PrintCommStatus(commStatus);
            //return -1;
        }
    }
    return -1;
}

//获取单个舵机单字节属性
//读取成功，返回大于0的值;读入失败, 返回-1；
int get_one_servo_byte(int id, int address)
{
    int value, commStatus, num = 0;
    while(1) {
        value = dxl_read_byte(id, address);
        commStatus = dxl_get_result();
        if((commStatus == COMM_RXSUCCESS) && (value < 255) && (value >= 0)) {
            PrintErrorCode();
            break;
            //return value;
        }
        else {
            PrintCommStatus(commStatus);
            printf("-->get_one_servo_byte:servo(%d) errorNum(%d)\n", id, num++);fflush(stdout);
            dxl_hal_clear();//清空数据包
            //usleep(10);
        }
    }
    return value;
}

//获取单个舵机多字节属性
//读取成功，返回大于0的值;读入失败, 返回-1；
int get_one_servo_word(int id, int address)
{
    int value, commStatus, num = 0;
    while(1) {
        value = dxl_read_word(id, address);
        commStatus = dxl_get_result();
        if((COMM_RXSUCCESS == commStatus) && (value < 4096) && (value >= 0)) {
            PrintErrorCode();
            break;
        }
        else {
            PrintCommStatus(commStatus);
            printf("-->get_one_servo_word:servo(%d) errorNum(%d)\n", id, num++);fflush(stdout);
            dxl_hal_clear();//清空数据包
        }
    }

    return value;
}

//等待1个舵机运动停止
//执行成功返回1；执行失败返回-1
int wait_for_one_servo(int id)
{
    int moving, value, read_num = 0;
    
    value = get_one_servo_word(id, Goal_Position);
    printf("MX28AT::wait_for_one_servo:id(%d) objK(%d)\n", id, value);

    do{
        moving = get_one_servo_byte(id, Moving); //获取运动结果
        if(0 == moving) {
        	read_num++;
        }
        else
        	read_num = 0;

    }while(read_num <= 10); //Goal_Position正在执行
    
    //读取舵机的当前刻度
    value = get_one_servo_word(id, Present_Position);
	printf("MX28AT::wait_for_one_servo:id(%d) preK(%d)\n", id, value);
  
    return 1;
}

//当舵机当前刻度到达exten范围则停止
int wait_for_one_servo_exten(int id, int exten)
{
    int goal_pos, value, diff;

    //读取目标刻度
    goal_pos = get_one_servo_word(id, Goal_Position);
//    printf("MX28AT::wait_for_one_servo_exten:servo(%d) Goal_Position(%d)\n", id, goal_pos);

    do{
        value = get_one_servo_word(id, Present_Position);
//        printf("MX28AT::wait_for_one_servo_exten:servo(%d) Present_Position(%d)\n", id, value);
        diff = goal_pos - value;
        if(diff <= 0) diff = (-1) * diff;
    }while(diff >= exten);

    return 1;
}

//控制舵机的demo
void servo_control_demo()
{
    char choice;
    int id[3], value1[3], value2[3];
    printf("欢迎进入控制舵机的demo...\n");fflush(stdout);
    while(1) {
        printf("请选择控制模式:q:退出; s:单个舵机；m:控制多个舵机\n");
        fflush(stdout);getchar();fflush(stdin);
        if(0 == scanf("%c", &choice)) return ;
        if('q' == choice) {
            printf("正在退出舵机控制模式...\n");fflush(stdout);
            break;
        }
        else if('s' == choice) {
            printf("欢迎进入单舵机控制模式\n");
            printf("请输入舵机ID:");fflush(stdout);
            if(0 == scanf("%d", &id[0])) return;fflush(stdin);
            value1[0] = get_one_servo_word(id[0], Present_Position);
            printf("%d舵机的当前位置%d\n", id[0], value1[0]);
            printf("请输入舵机目标刻度和运动速度:");fflush(stdout);
            if(0 == scanf("%d %d", &value1[0], &value2[0])) return;
            set_one_servo_word(id[0], Moving_Speed, value2[0]); //设置速度
            set_one_servo_word(id[0], Goal_Position, value1[0]);//设置目标位置
            wait_for_one_servo(id[0]); //等待舵机运动结束
        }
        else if('m' == choice) {
            printf("欢迎进入单舵机控制模式\n");
            printf("请输入舵机id1, id2, id3:");fflush(stdout);
            if(0 ==scanf("%d %d %d", &id[0], &id[1], &id[2])) return;fflush(stdin);
            //获得当前位置
            value1[0] = get_one_servo_word(id[0], Present_Position); //
            value1[1] = get_one_servo_word(id[1], Present_Position);
            value1[2] = get_one_servo_word(id[2], Present_Position);
            printf("%d舵机当前位置%d\n", id[0], value1[0]);
            printf("%d舵机当前位置%d\n", id[1], value1[1]);
            printf("%d舵机当前位置%d\n", id[2], value1[2]);
            printf("请输入舵机速度sp1, sp2, sp3:");fflush(stdout);
            if(0 == scanf("%d %d %d", &value1[0], &value1[1], &value1[2])) return;fflush(stdin);
            set_servo_num(3);//设置舵机个数
            set_servo_unit(id);//设置舵机组
            set_many_servo_word(Moving_Speed, value1);//设置速度
            printf("请输入舵机目标刻度gp1, gp2, gp3:");fflush(stdout);
            if(0 == scanf("%d %d %d", &value2[0], &value2[1], &value2[2])) return;fflush(stdin);
            set_many_servo_word(Goal_Position, value2);//设置目标位置
            wait_for_one_servo(id[0]);//等待结束
            wait_for_one_servo(id[1]);
            wait_for_one_servo(id[2]);
        }
        else {
        }
    }
}
