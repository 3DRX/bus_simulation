#include <stdio.h>
#include <stdlib.h>
#include "main.h"

extern ENVIRONMENT env;

void readfile(FILE * fPtr)
{// 从文件中读取环境初始配置
    env.TOTAL_STATION = 5;
    env.STRATEGY = FCFS;
    env.DISTANCE = 2;
    // start read file
    // end read file
}
