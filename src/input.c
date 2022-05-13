#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"

extern ENVIRONMENT env;

void readfile(FILE * fPtr) // 从文件中读取环境初始配置
{
    int workingIndex = 0;
    enum {COMMENT, NORMAL, TOTAL_STATION, STRATEGY, DISTANCE} state;
    char * lineBuffer = (char *)malloc(sizeof(char)* 100);

    env.TOTAL_STATION = 5;
    env.STRATEGY = FCFS;
    env.DISTANCE = 2;
    // start read file
    char ipt;
    state = NORMAL;
    while (1) {
        if ((fscanf(fPtr, "%c", &ipt)) == EOF) {
            //printf("!! file end\n");
            //printf("!! TOTAL_STATION is %d\n",env.TOTAL_STATION);
            //printf("!! DISTANCE is %d\n",env.DISTANCE);
            break;
        }
        else {
            workingIndex++;
        }
        if (state == NORMAL) {
            //printf("!! enter state: NORMAL\n");
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
            //printf("!! enter state: COMMENT\n");
            if (ipt == '\n') {
                state = NORMAL;
                workingIndex = 0;
            }
        }
        else if (state == TOTAL_STATION) {
            //printf("!! enter state: TOTAL_STATION\n");
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.TOTAL_STATION);
                //printf("!! TOTAL_STATION changed to %d\n",env.TOTAL_STATION);
                state = NORMAL;
            }
        }
        else if (state == STRATEGY) {
            //printf("!! enter state: STRATEGY\n");
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
            //printf("!! enter state: DISTANCE\n");
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.DISTANCE);
                //printf("!! DISTANCE changed to %d\n",env.DISTANCE);
                state = NORMAL;
            }
        }
    }

    // end read file
}

