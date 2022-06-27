#include "core.h"
#include "input.h"
#include "main.h"
#include "mainwindow.h"
#include "output.h"
#include "strategy.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>

#include <QApplication>

// #variables (global)
ENVIRONMENT env;
CAR car;
STATION station;
int TIME = 0;

void initGame(void)
{
    //打开输入输出文件
    if ((env.input = fopen("input.io", "r")) == NULL) {
        printf("Can't open file \"input.io\"\n");
        exit(EXIT_FAILURE);
    }
    if ((env.output = fopen("output.io", "w")) == NULL) {
        printf("Can't open file \"output.io\"\n");
        exit(EXIT_FAILURE);
    }
    // init car & station
    // 使用 env.TOTAL_STATION 令输出长度可变（如果规则要求输出长度永远是10
    // 就把本函数中所有的 env.TOTAL_STATION 替换为10）
    car.position = 0;
    for (int i = 0; i < 20; i++) {
        car.target[0][i] = 0;
        station.clockwise[0][i] = 0;
        station.counterclockwise[0][i] = 0;
        // 将数组的第二行全部置0
        car.target[1][i] = 0;
        station.clockwise[1][i] = 0;
        station.counterclockwise[1][i] = 0;
    }
    car.target[0][env.TOTAL_STATION] = -1;
    station.clockwise[0][env.TOTAL_STATION] = -1;
    station.counterclockwise[0][env.TOTAL_STATION] = -1;
    // 上面三行中 -1 表示行结束（类似'\0'）
    // 如果是FCFS模式，初始化链表
    if (env.STRATEGY == ENVIRONMENT::FCFS) {
        env.headnode = (NODE*)malloc(sizeof(NODE));
        env.headnode->prev = NULL;
        env.headnode->next = NULL;
        env.headnode->stationNumber = -1;
        env.headnode->where = -1;
        env.presentPtr = env.headnode;
    } else {
        env.headnode = NULL;
    }
    // 初始化env.presentWorkingPtr
    env.presentWorkingPtr = NULL;
    // 输出TIME: 0
    printLines();
}

/**当输入检测到end之后，mainLoop会结束
 */
void mainLoop()
{
    if (env.STRATEGY == ENVIRONMENT::FCFS) {
        // FCFS_readOrder();
    } else {
        // readOrder();
    }
    strategy();
    outPut();
}

CoreThread::CoreThread() { }

void CoreThread::run()
{
    // 后台（核心部分）入口函数
    initGame();
    while (1) {
        mainLoop();
    }
    fclose(env.input);
    fclose(env.output);
    return;
}
