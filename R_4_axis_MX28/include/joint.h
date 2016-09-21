#ifndef _JOINT_HEADER
#define _JOINT_HEADER

#ifdef __cplusplus
extern "C" {
#endif

void set_joint_num(int num); //设置舵机个数
void set_joint_unit(int joint_unit_temp[]); //设置舵机组
int set_one_joint_angle_spe(int joint_id, float angle_spe);//设置单个舵机运动速度
int set_one_joint_goal_angle(int joint_id, float goal_angle); //设置单个舵机目标角度
int set_many_joint_angle_spe(float angle_spe[]);//设置多个舵机多自己属性
int set_many_joint_goal_angle(float goal_angle[]);//设置多个舵机多自己属性
float get_one_joint_angle_spe(int joint_id);//获取单个关节目标速度
float get_one_joint_goal_angle(int joint_id); //获取单个关节目标角度
int wait_for_one_joint(int joint_id); //等待单个关节停止
int wait_for_one_joint_exten(int joint_id); //等待某个关节角度到达一定范围


void joint_control_demo(); //控制关节的demo

#ifdef __cplusplus
}
#endif

#endif
