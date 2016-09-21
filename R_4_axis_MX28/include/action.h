#ifndef _ACTION_HEADER
#define _ACTION_HEADER

#ifdef __cplusplus
extern "C" {
#endif

int initail_system();//初始化系统

int initial_pos(float x, float y, float z); //初始化端点D在棋盘中的位置
void set_spe(float obj_xyz[], float t);     //设置运动速度
void move_to_xyz(float obj_xyz[]);          //以设置的速度运动到目标位置
void move_to_xyz_t(float obj_xyz[]);        //以设置的时间从当前位置运动到目标位置
void move_to_xyz_t1(float obj_xyz[], float t); //以设置的时间从当前位置运动到目标位置

//基本动作控制
void up_dowm_move(float add_height, float t); //上下运动基于当前量
void up_down_move_1(float add_height, float t); //上下运动基于当前量
void up_down_move_2(float add_height, float t); //上下运动基于当前量
//void forward_back_move(float add_length); //前进后退运动基于当前量
//void left_right_move(float add_right); //左右运动基于当前量

//控制移动到目标点的demo
void xyz_control_demo(); //控制端点D的demo

#ifdef __cplusplus
}
#endif

#endif
