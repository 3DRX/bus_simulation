#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

extern ENVIRONMENT env;

void readfile(FILE * fPtr) // 从文件中读取环境初始配置
{
    int workingIndex = 0;
    // 自动机的状态
    enum {COMMENT, NORMAL, TOTAL_STATION, STRATEGY, DISTANCE} state;
    state = NORMAL;
    env.TOTAL_STATION = 5;
    env.STRATEGY = FCFS;
    env.DISTANCE = 2;
    char ipt;
    // start read file
    while (1) {
        if ((fscanf(fPtr, "%c", &ipt)) == EOF) {
            break;
        }
        else {
            workingIndex++;
        }
        if (state == NORMAL) {
            if (workingIndex == 1 && ipt == '#') {
                state = COMMENT;
            }
            else if (workingIndex == 1 && ipt == 'T') {
                state = TOTAL_STATION;
            }
            else if (workingIndex == 1 && ipt == 'S') {
                state = STRATEGY;
            }
            else if (workingIndex == 1 && ipt == 'D') {
                state = DISTANCE;
            }
            else if (ipt == '\n') {
                workingIndex = 0;
            }
        }
        else if (state == COMMENT) {
            if (ipt == '\n') {
                state = NORMAL;
                workingIndex = 0;
            }
        }
        else if (state == TOTAL_STATION) {
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.TOTAL_STATION);
                state = NORMAL;
            }
        }
        else if (state == STRATEGY) {
            if (ipt == '=') {
                char inputBuf[10];
                fscanf(fPtr, "%s", inputBuf);
                if (strcmp(inputBuf, "FCFS") == 0) {
                    env.STRATEGY = FCFS;
                }
                else if (strcmp(inputBuf, "SSTF") == 0) {
                    env.STRATEGY = SSTF;
                }
                else if (strcmp(inputBuf, "SCAN") == 0) {
                    env.STRATEGY = SCAN;
                }
                state = NORMAL;
            }
        }
        else if (state == DISTANCE) {
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.DISTANCE);
                state = NORMAL;
            }
        }
    }
    // end read file
}

