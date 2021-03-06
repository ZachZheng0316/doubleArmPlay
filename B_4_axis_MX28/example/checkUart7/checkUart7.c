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

//全局函数
int initial_sys(int device, int baudNum); //系统初始化函数

void receive_instructor(unsigned char instruct[]); //获取指令
int execute_instructor(unsigned char instruct_char[]); //解析并执行指令
void finish_instructor();  //发送完成信号

int main()
{
	unsigned char packet[20] = {0, }; //存储接受的指令
	
    //初始化
	if(1 != initial_sys(5, 1200)) {
		printf("error-->double_4-axis::main:initial_sys() failed\n");
		return -1;
	}
	else{
		printf("sys init success...\n");
		//getchar();
	}
    
    do{
    	receive_instructor(packet); //接受指令
    	
    	finish_instructor();        //发送完成信号
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
			instruct[flag] = '\0';
			printf("the receiving data (%s)\n", instruct); fflush(stdout);
			break;
		}
		else {
		}
	}
}

//发送完成信号
void finish_instructor()
{
	int flag;
	unsigned char return_packet[4] = "okk";
		
    flag = sendMessage(return_packet, 3);
	if(flag != 3) {
		printf("failed send data\r"); fflush(stdout);
	}
	else {
		printf("success send data:%s\n", return_packet); fflush(stdout);
	}	
}

//初始化
int initial_sys(int device, int baudNum)
{
    //打开通信串口
    if(1 != serial_open(device, baudNum)) {
        printf("double_4-axis::initail_sys:serial_open failed\n");
        return -1;
    }
   	
    return 1;
}

