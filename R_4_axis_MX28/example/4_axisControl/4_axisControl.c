#include "demo.h"
#include <stdio.h>

int main()
{
    //delta控制函数

    if(-1 == delta_control()) {
        printf("deltaControl::main:failed delta_control\n");
        return -1;
    }

    //cal();

    return 1;
}
