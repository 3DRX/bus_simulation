#include <stdio.h>
#include <stdlib.h>
#include "main.h"

extern ENVIRONMENT env;

void readfile(FILE * fPtr) // 从文件中读取环境初始配置
{
    char station[] = "TOTAL_STATION";
    char strategy[] = "STRATEGY";
    char distance[] = "DISTANCE";
    int workingIndex = 0;
    enum {COMMENT, NORMAL} state;
    char * lineBuffer = (char *)malloc(sizeof(char)* 100);

    env.TOTAL_STATION = 5;
    env.STRATEGY = FCFS;
    env.DISTANCE = 2;
    // start read file
    char ipt;
    state = NORMAL;
    while (1) {
        if ((fscanf(fPtr, "%c", &ipt)) == EOF) {
            break;
        }
        if (state == NORMAL) {
            if (workingIndex == 0 && ipt == '#') {
                state = COMMENT;
            }
        }
        else if (state == COMMENT) {
            if (ipt == '\n') {
                state = NORMAL;
            }
        }
    }

    // end read file
}
