#include "action.h"
#include "MX28AT.h"
#include "dmath.h"
#include "joint.h"
#include "dynamixel.h"
#include "algorithm.h"
#include <stdio.h>
#include <math.h>
#include <unistd.h>

//全局变量
static float Current_xyz[3];//端点D的当前位置

//初始化成功，返回1； 初始化失败， 返回-1;
int initail_system()
{
    int deviceIndex = 0, baudnum = 207;
    int num = 3, ids[3] = {1, 2, 3}, value[3];
    int p_gain = 46, i_gain = 32, d_gain = 30;

    printf( "\n\ninitail_system\n\n" );
    //初始化泵
//    open_port(); close_beng();
    printf("the deviceIndex and baudnum(default 0 and 1):");fflush(stdout);//setbuf(stdin, NULL);
/*
    if(0 == scanf("%d %d", &deviceIndex, &baudnum)) {
        printf("action::initail_system:failed to input\n");
        return -1;
    }
*/
    printf("deviceIndex(%d), baudnum(%d)\n", deviceIndex, baudnum);fflush(stdout);
	///////// Open USB2Dynamixel ////////////
	if( dxl_initialize(deviceIndex, baudnum) == 0 )
	{
		printf( "Failed to open USB2Dynamixel!\n" );
		printf( "Press Enter key to terminate...\n" );
		getchar();
		return 0;
	}
	else {
		printf( "Succeed to open USB2Dynamixel!\n" );
        fflush(stdout); 
//        getchar();
    }

    //初始化舵机组
    set_servo_num(num); //设置舵机个数
    set_servo_unit(ids);

    //初始化舵机舵机PID
    value[0] = p_gain; value[1] = p_gain; value[2] = p_gain;
    if(-1 == set_many_servo_byte(P_Gain, value)) {
        printf("action.c::initail_system:failed to P_Gain\n");
        return -1;
    }
    value[0] = i_gain; value[1] = i_gain; value[2] = i_gain;
    if(-1 == set_many_servo_byte(I_Gain, value)) {
        printf("action.c::initail_system:failed to I_Gain\n");
        return -1;
    }
    value[0] = d_gain; value[1] = d_gain; value[2] = d_gain;
    if(-1 == set_many_servo_byte(D_Gain, value)) {
        printf("action.c::initail_system:failed to D_Gain\n");
        return -1;
    }

    //初始化舵机速度
    /*
    value[0] = 200; value[1] = 200; value[2] = 200;
    if(-1 == set_many_servo_word(Moving_Speed, value)) {
        printf("action.c::initail_system:failed to servo speed\n");
        return -1;
    }
    */
    //初始化舵机位置
    /*
    value[0] = ID1POSK; value[1] = ID2POSK + 455; value[2] = ID3POSK - 909;
    if(-1 == set_many_servo_word(Goal_Position, value)) {
        printf("action.c::initail_system:failed to servo init position\n");
        return -1;
    }

    //等待舵机运动结束
    wait_for_one_servo(ids[0]);
    wait_for_one_servo(ids[1]);
    wait_for_one_servo(ids[2]);
    */
    initial_pos(0.0, 1.0, 62.70);

    return 1;
}

//初始化端点D在棋盘中的位置
int initial_pos(float x, float y, float z)
{
    int joint_unit[3];
    float joint_rad[3], joint_angle[3], angle_spe[3];

    //端点D在棋盘中坐标转化为关节弧度
    set_Dxyzboard(x, y, z);//设置端点D在棋盘中的位置
    trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
    get_jointradian(joint_rad);

    //关节弧度转化为关节角度
    joint_angle[0] = AngleFromPI(joint_rad[0]);
    joint_angle[1] = AngleFromPI(joint_rad[1]);
    joint_angle[2] = AngleFromPI(joint_rad[2]);
    printf("action::initial_pos:(%f, %f, %f)\n", joint_angle[0], joint_angle[1], joint_angle[2]);

    //控制关节运动到相应的角度
    joint_unit[0] = 1; joint_unit[1] = 2; joint_unit[2] = 3;
    angle_spe[0] = 30.0; angle_spe[1] = 30.0; angle_spe[2] = 30.0;
    set_joint_num(3);//设置关节个数
    set_joint_unit(joint_unit);//设置关节ID
    set_many_joint_angle_spe(angle_spe);//设置关节运动速度
    set_many_joint_goal_angle(joint_angle);//控制关节运动到目标角度

    //等待运动结束
    wait_for_one_joint(joint_unit[0]);
    wait_for_one_joint(joint_unit[1]);
    wait_for_one_joint(joint_unit[2]);

    //更新当前坐标
    Current_xyz[0] = x;
    Current_xyz[1] = y;
    Current_xyz[2] = z;

    printf("\naction::initial_pos:success\n");

    return 1;
}

//设置运动速度
void set_spe(float obj_xyz[], float t)
{
    float joint_spe_rad[3], joint_spe_angle[3], rate = 0.5;
    //计算运动速度
    set_AspeO(Current_xyz, obj_xyz, t); //设置点A在O系中的速度
    cal_jointsperadian();//计算关节弧速度
    get_jointsperadian(joint_spe_rad); //获取关节弧速度

    //关节弧速度转化为关节角速度
    joint_spe_angle[0] = AngleFromPI(joint_spe_rad[0]);
    joint_spe_angle[1] = AngleFromPI(joint_spe_rad[1]);
    joint_spe_angle[2] = AngleFromPI(joint_spe_rad[2]);
	
	if(fabs(joint_spe_angle[1]) >= fabs(joint_spe_angle[2])) {
		if(fabs(joint_spe_angle[2]) <= fabs(joint_spe_angle[1]) * rate)
			joint_spe_angle[2] = fabs(joint_spe_angle[1]) * rate * joint_spe_angle[2] / joint_spe_angle[2];
	}
	else {
		if(fabs(joint_spe_angle[1]) <= fabs(joint_spe_angle[2]) * rate) 
			joint_spe_angle[1] = fabs(joint_spe_angle[2]) * rate * joint_spe_angle[1] / joint_spe_angle[1];
	} 	
    //设置运动速度
    //省略设置关节个数和ID组
    set_many_joint_angle_spe(joint_spe_angle); //设置关节速度
    printf("action::set_spe:(%f, %f, %f)\n", joint_spe_angle[0], joint_spe_angle[1], joint_spe_angle[2]);
}

//以设置的速度运动到目标位置
void move_to_xyz(float obj_xyz[])
{
    float joint_rad[3], joint_angle[3];

    //计算目标刻度
    set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
    trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
    get_jointradian(joint_rad); //获得当前关节弧度

    //关节弧度转化为关节角度
    joint_angle[0] = AngleFromPI(joint_rad[0]);
    joint_angle[1] = AngleFromPI(joint_rad[1]);
    joint_angle[2] = AngleFromPI(joint_rad[2]);

    //设置目标刻度
    //省略设置关节个数和ID组
    set_many_joint_goal_angle(joint_angle);

    //等待运动结束
    wait_for_one_joint_exten(1);
    wait_for_one_joint_exten(2);
    wait_for_one_joint_exten(3);

    //更新当前坐标
    Current_xyz[0] = obj_xyz[0];
    Current_xyz[1] = obj_xyz[1];
    Current_xyz[2] = obj_xyz[2];

    printf("action::move_to_xyz:success(%f, %f, %f)\n", Current_xyz[0], Current_xyz[1], Current_xyz[2]);
}

//以设置的时间从当前位置运动到目标位置
void move_to_xyz_t(float obj_xyz[])
{
    float joint_rad_start[3], joint_rad_end[3];
    float joint_spe[3], t_unit = 0.12, dis_unit = 1.0, t, dis;

    //设置运动时间
    dis = pow(fabs(obj_xyz[0] - Current_xyz[0]), 2);
    dis += pow(fabs(obj_xyz[1] - Current_xyz[1]), 2);
    dis = sqrt(dis);
    t = (dis * t_unit ) / dis_unit;
    if(t <= 0.0) t = 1.0;
    printf("distance(%f), time(%f)\n", dis, t);

    //获得当前关节弧度
    get_jointradian(joint_rad_start);

    //计算目标点的关节弧度
    set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
    trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
    get_jointradian(joint_rad_end); //获得当前关节弧度

    //计算关节速度
    joint_spe[0] = (joint_rad_end[0] - joint_rad_start[0]) / t;
    joint_spe[1] = (joint_rad_end[1] - joint_rad_start[1]) / t;
    joint_spe[2] = (joint_rad_end[2] - joint_rad_start[2]) / t;
    joint_spe[0] = AngleFromPI(joint_spe[0]);
    joint_spe[1] = AngleFromPI(joint_spe[1]);
    joint_spe[2] = AngleFromPI(joint_spe[2]);

    //计算目标点的关节角度
    joint_rad_end[0] = AngleFromPI(joint_rad_end[0]);
    joint_rad_end[1] = AngleFromPI(joint_rad_end[1]);
    joint_rad_end[2] = AngleFromPI(joint_rad_end[2]);

    //设置关节速度
    set_many_joint_angle_spe(joint_spe); //设置关节速度
    printf("action::move_to_xyz_t:(%f, %f, %f)\n", joint_spe[0], joint_spe[1], joint_spe[2]);

    //设置关节角度
    set_many_joint_goal_angle(joint_rad_end);
    printf("action::move_to_xyz_t:success(%f, %f, %f)\n", joint_rad_end[0], joint_rad_end[1], joint_rad_end[2]);

    //等待运动停止
    wait_for_one_joint(1);
    wait_for_one_joint(2);
    wait_for_one_joint(3);

    //更新当前坐标
    Current_xyz[0] = obj_xyz[0];
    Current_xyz[1] = obj_xyz[1];
    Current_xyz[2] = obj_xyz[2];
}

//以设置的时间从当前位置运动到目标位置
void move_to_xyz_t1(float obj_xyz[], float t)
{
	float joint_rad_start[3], joint_rad_end[3];
    float joint_spe[3], rate = 0.67;

    //获得当前关节弧度
    get_jointradian(joint_rad_start);

    //计算目标点的关节弧度
    set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
    trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
    get_jointradian(joint_rad_end); //获得当前关节弧度

    //计算关节速度
    joint_spe[0] = (joint_rad_end[0] - joint_rad_start[0]) / t;
    joint_spe[1] = (joint_rad_end[1] - joint_rad_start[1]) / t;
    joint_spe[2] = (joint_rad_end[2] - joint_rad_start[2]) / t;
    joint_spe[0] = AngleFromPI(joint_spe[0]);
    joint_spe[1] = AngleFromPI(joint_spe[1]);
    joint_spe[2] = AngleFromPI(joint_spe[2]);

    //计算目标点的关节角度
    joint_rad_end[0] = AngleFromPI(joint_rad_end[0]);
    joint_rad_end[1] = AngleFromPI(joint_rad_end[1]);
    joint_rad_end[2] = AngleFromPI(joint_rad_end[2]);
    
    
    //防止2号机械臂与3号机械臂速度差别过大
	if(fabs(joint_spe[1]) >= fabs(joint_spe[2])) {
		if(fabs(joint_spe[2]) <= fabs(joint_spe[1]) * rate)
			joint_spe[2] = fabs(joint_spe[1]) * rate * joint_spe[2] / joint_spe[2];
	}
	else {
		if(fabs(joint_spe[1]) <= fabs(joint_spe[2]) * rate) 
			joint_spe[1] = fabs(joint_spe[2]) * rate * joint_spe[1] / joint_spe[1];
	}

    //设置关节速度
    set_many_joint_angle_spe(joint_spe); //设置关节速度
//    printf("action::move_to_xyz_t1:(joint_spe)(%f, %f, %f)\n", joint_spe[0], joint_spe[1], joint_spe[2]);

    //设置关节角度
    set_many_joint_goal_angle(joint_rad_end);
//    printf("action::move_to_xyz_t1:(joint_rad_end)(%f, %f, %f)\n", joint_rad_end[0], joint_rad_end[1], joint_rad_end[2]);

    //等待运动停止
    //wait_for_one_joint_exten(1);
    //wait_for_one_joint_exten(2);
    //wait_for_one_joint_exten(3);

    //更新当前坐标
    Current_xyz[0] = obj_xyz[0];
    Current_xyz[1] = obj_xyz[1];
    Current_xyz[2] = obj_xyz[2];
}

//基本动作控制
//上下运动基于当前量
//当add_height>=0, 说明向上运动
//当add_height<0, 说明向下运动
void up_dowm_move(float add_height, float t)
{
    float height, add_step, t_step, Current_z;
    float obj_xyz[3];

    if(add_height >= 0.0)
        add_step = 10.0;
    else
        add_step = (-10.0);

    //计算add_step运动时间
    t_step = fabs((t * add_step ) / add_height);
    if(t_step >= 100.0) t_step = 100.0; //
    if(t_step <= 0.01) t_step = 0.01;

    //计算起始速度
    obj_xyz[0] = Current_xyz[0];
    obj_xyz[1] = Current_xyz[1];
    obj_xyz[2] = Current_xyz[2] + add_step;
    set_spe(obj_xyz, t_step); //设置起始运动速度

    //向上运动
    Current_z = Current_xyz[2];
    if(add_height >= 0.0) {
        for(height = Current_z + add_step; height < Current_z + add_height; height += add_step) {
            //计算目标位置
            obj_xyz[2] = height;
            move_to_xyz(obj_xyz); //以设置的速度运动到目标位置

            //计算此时速度
            obj_xyz[2] = height + add_step;
            set_spe(obj_xyz, t_step); //设置运动速度
        }
    }
    else { //向下运动
        for(height = Current_z + add_step; height > Current_z + add_height; height += add_step) {
            //计算目标位置
            obj_xyz[2] = height;
            move_to_xyz(obj_xyz); //以设置的速度运动到目标位置

            //计算此时速度
            obj_xyz[2] = height + add_step;
            set_spe(obj_xyz, t_step); //设置运动速度
        }
    }
    //计算最后的目标位置
    obj_xyz[2] = Current_z + add_height;
    move_to_xyz(obj_xyz); //以设置的速度运动到目标位置
}

//上下运动基于当前量
//先计算运动数据，再运动
void up_down_move_1(float add_height, float t)
{
    float height, add_step, t_step, Current_z;
    float obj_xyz[3], dis_nodes[25], time_nodes[25];
    float spe_array[25][3], obj_array[25][3];
    float joint_spe[3], joint_rad[3], current_xyz[3];
    int node_num = 0, i = 0;

    if(add_height >= 0.0)
        add_step = 5.0;
    else
        add_step = (-5.0);

    //计算add_step运动时间
    t_step = fabs((t * add_step ) / add_height);
    if(t_step >= 100.0) t_step = 100.0; //
    if(t_step <= 0.1) t_step = 0.1;

    //向上运动
    Current_z = Current_xyz[2];//记录当前高度
    if(add_height >= 0.0) {
        for(height = Current_z + add_step; height < Current_z + add_height; height += add_step) {
            dis_nodes[node_num] = height;
            time_nodes[node_num] = t_step;
            node_num ++;
        }
    }
    else { //向下运动
        for(height = Current_z + add_step; height > Current_z + add_height; height += add_step) {
            dis_nodes[node_num] = height;
            time_nodes[node_num] = t_step;
            node_num ++;
        }
    }

    //计算最后的目标位置
    dis_nodes[node_num] = Current_z + add_height;
    add_step = (Current_z + add_height) - (height - add_step);
    t_step = fabs((t * add_step ) / add_height);
    if(t_step >= 100.0) t_step = 100.0; //
    if(t_step <= 0.1) t_step = 0.1;
    time_nodes[node_num] = t_step;//计算最后一个节点的时间

    //计算节点数据
    current_xyz[0] = Current_xyz[0];
    current_xyz[1] = Current_xyz[1];
    current_xyz[2] = Current_xyz[2];
    obj_xyz[0] = Current_xyz[0];
    obj_xyz[1] = Current_xyz[1];
    for(i = 0; i <= node_num; i++) {
        obj_xyz[2] = dis_nodes[i];
        printf("dis_nodes[%d]:%f, time_nodes[%d]:%f\n", i, dis_nodes[i], i, time_nodes[i]);

        //计算速度节点
        set_AspeO(current_xyz, obj_xyz, time_nodes[i]); //设置点A在O系中的速度
        cal_jointsperadian();//计算关节弧速度(需要当前关节弧度)
        get_jointsperadian(joint_spe); //获取关节弧速度
        //关节弧速度转化为关节角速度
        spe_array[i][0] = AngleFromPI(joint_spe[0]);
        spe_array[i][1] = AngleFromPI(joint_spe[1]);
        spe_array[i][2] = AngleFromPI(joint_spe[2]);

        //计算目标刻度
        set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
        trans_Dxyzboard_Dxyzabs();						  //点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
        trans_Dxyzabs_AxyzO();							  //点D在绝对坐标系中的坐标转化为点A在O系中的坐标
        cal_AxyzO_jointradian();						  //点A在O系中的坐标转化为关节弧度
        get_jointradian(joint_rad); 					  //获得当前的关节弧度

        //关节弧度转化为关节角度
        obj_array[i][0] = AngleFromPI(joint_rad[0]);
        obj_array[i][1] = AngleFromPI(joint_rad[1]);
        obj_array[i][2] = AngleFromPI(joint_rad[2]);
    }
    //执行数据流
    for(i = 0; i <= node_num; i++) {
        //设置速度
        set_many_joint_angle_spe(spe_array[i]);
        //设置目标刻度
        set_many_joint_goal_angle(obj_array[i]);
        //等待运动结束
        //wait_for_one_joint_exten(2);
        //wait_for_one_joint_exten(3);
        delay_us(50000); //50ms
    }

    //等待运动结束
    //wait_for_one_joint_exten(2);
    //wait_for_one_joint_exten(3);
    delay_us(100000); //延迟500ms

    //更新当前坐标
    Current_xyz[2] = Current_z + add_height;
}

//上下运动基于当前量
void up_down_move_2(float add_height, float t)
{
    float height, add_step, t_step, Current_z;
    float obj_xyz[3], joint_spe_rad[3], joint_spe_angle[3];
    float joint_angle[3], joint_rad[3];

    if(add_height >= 0.0)
        add_step = 20.0;
    else
        add_step = (-20.0);

    //计算add_step运动时间
    t_step = fabs((t * add_step ) / add_height);
    if(t_step >= 100.0) t_step = 100.0; //
    if(t_step <= 0.01) t_step = 0.01;

    //计算起始速度
    obj_xyz[0] = Current_xyz[0];
    obj_xyz[1] = Current_xyz[1];
    obj_xyz[2] = Current_xyz[2] + add_step;
    set_AspeO(Current_xyz, obj_xyz, t_step); //设置点A在O系中的速度
    cal_jointsperadian();//计算关节弧速度
    get_jointsperadian(joint_spe_rad); //获取关节弧速度
    //关节弧速度转化为关节角速度
    joint_spe_angle[0] = AngleFromPI(joint_spe_rad[0]);
    joint_spe_angle[1] = AngleFromPI(joint_spe_rad[1]);
    joint_spe_angle[2] = AngleFromPI(joint_spe_rad[2]);

    //向上运动
    Current_z = Current_xyz[2]; //记录当前的高度
    if(add_height >= 0.0) {
        for(height = Current_z + add_step; height < Current_z + add_height; height += add_step) {
            //计算目标位置
            obj_xyz[2] = height;
            //计算目标刻度
            set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
            trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
            trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
            cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
            get_jointradian(joint_rad); //获得当前关节弧度
            //关节弧度转化为关节角度
            joint_angle[0] = AngleFromPI(joint_rad[0]);
            joint_angle[1] = AngleFromPI(joint_rad[1]);
            joint_angle[2] = AngleFromPI(joint_rad[2]);

            //等待运动停止
            wait_for_one_joint(2);
            wait_for_one_joint(3);

            //设置速度和目标位置
            set_many_joint_angle_spe(joint_spe_angle); //设置关节速度
            set_many_joint_goal_angle(joint_angle);

            //计算此时速度
            Current_xyz[2] = height;
            obj_xyz[2] = height + add_step;
            set_AspeO(Current_xyz, obj_xyz, t_step); //设置点A在O系中的速度
            cal_jointsperadian();//计算关节弧速度
            get_jointsperadian(joint_spe_rad); //获取关节弧速度
            //关节弧速度转化为关节角速度
            joint_spe_angle[0] = AngleFromPI(joint_spe_rad[0]);
            joint_spe_angle[1] = AngleFromPI(joint_spe_rad[1]);
            joint_spe_angle[2] = AngleFromPI(joint_spe_rad[2]);
        }
    }
    else { //向下运动
        for(height = Current_z + add_step; height > Current_z + add_height; height += add_step) {
            //计算目标位置
            obj_xyz[2] = height;
            //计算目标刻度
            set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
            trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
            trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
            cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
            get_jointradian(joint_rad); //获得当前关节弧度
            //关节弧度转化为关节角度
            joint_angle[0] = AngleFromPI(joint_rad[0]);
            joint_angle[1] = AngleFromPI(joint_rad[1]);
            joint_angle[2] = AngleFromPI(joint_rad[2]);

            //等待运动停止
            wait_for_one_joint(2);
            wait_for_one_joint(3);

            //设置速度和目标位置
            set_many_joint_angle_spe(joint_spe_angle); //设置关节速度
            set_many_joint_goal_angle(joint_angle);

            //计算此时速度
            Current_xyz[2] = height;
            obj_xyz[2] = height + add_step;
            set_AspeO(Current_xyz, obj_xyz, t_step); //设置点A在O系中的速度
            cal_jointsperadian();//计算关节弧速度
            get_jointsperadian(joint_spe_rad); //获取关节弧速度
            //关节弧速度转化为关节角速度
            joint_spe_angle[0] = AngleFromPI(joint_spe_rad[0]);
            joint_spe_angle[1] = AngleFromPI(joint_spe_rad[1]);
            joint_spe_angle[2] = AngleFromPI(joint_spe_rad[2]);
        }
    }
    //计算最后的目标位置
    obj_xyz[2] = Current_z + add_height;
    set_Dxyzboard(obj_xyz[0], obj_xyz[1], obj_xyz[2]);//设置端点D在棋盘中的位置
    trans_Dxyzboard_Dxyzabs();//点D在棋盘中的坐标转化为点D在绝对坐标系中的坐标
    trans_Dxyzabs_AxyzO();//点D在绝对坐标系中的坐标转化为点A在O系中的坐标
    cal_AxyzO_jointradian();//点A在O系中的坐标转化为关节弧度
    get_jointradian(joint_rad); //获得当前关节弧度
    //关节弧度转化为关节角度
    joint_angle[0] = AngleFromPI(joint_rad[0]);
    joint_angle[1] = AngleFromPI(joint_rad[1]);
    joint_angle[2] = AngleFromPI(joint_rad[2]);

    //设置速度和目标位置
    set_many_joint_angle_spe(joint_spe_angle); //设置关节速度
    set_many_joint_goal_angle(joint_angle);

    //等待运动停止
    wait_for_one_joint(2);
    wait_for_one_joint(3);

    //更新当前坐标
    Current_xyz[2] = Current_z + add_height;
}

//void forward_back_move(float add_length); //前进后退运动基于当前量
//void left_right_move(float add_right); //左右运动基于当前量

//控制端点D的demo
void xyz_control_demo()
{
    char choice;
    float t, obj_xyz[3];
    printf("welcome enter control chess demo...\n");fflush(stdout);
    initial_pos(0.0, 4.0, 37.72);//位置初始化
    getchar();
    while(1) {
        printf("\nplease choose model:q,exit; c,control; u/d,up_dowm move:");
        fflush(stdout);
        choice = getchar(); //输入选项
        if('q' == choice) {
            printf("exiting ...\n");fflush(stdout);
            break;
        }
        else if('c' == choice) {
            printf("welcome enter chess control model\n");
            printf("D coordiante(%f, %f, %f)\n", Current_xyz[0], Current_xyz[1], Current_xyz[2]);
            printf("please input obj coordiante(xyz) and move time (t):");fflush(stdout);
            if(0 == scanf("%f %f %f %f", &obj_xyz[0], &obj_xyz[1], &obj_xyz[2], &t)) return;
            printf("\nthe getted value (%f %f %f)-->(%f)\n\n", obj_xyz[0], obj_xyz[1], obj_xyz[2], t);
            move_to_xyz_t1(obj_xyz, t);
        }
        else if(('u' == choice) || ('d' == choice)) {
            printf("welcome enter up_down move model\n");
            printf("D coordiante(%f, %f, %f)\n", Current_xyz[0], Current_xyz[1], Current_xyz[2]);
            printf("please input add_height(add_z) and move time(t):");fflush(stdout);
            //setbuf(stdin, NULL);
            if(0 == scanf("%f %f", &obj_xyz[2], &t)) return;
            up_dowm_move(obj_xyz[2], t);
        }
        else {
            printf("the choice is %c\n", choice);
        }
    }
//    close_beng();
}
