#ifndef _PLAYCHESS_HEADER
#define _PLAYCHESS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

struct XYZ{
    float x;
    float y;
    float z;
};

void init_chess_wait(); //初始化棋子待放区
void init_chess_pos(); //初始化棋子初始化位置


void put_chess(); //完成摆棋动作

#ifdef __cplusplus
}
#endif

#endif
