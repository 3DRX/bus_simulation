#include "core.h"
#include "input.h"
#include "main.h"
#include "mainwindow.h"
#include "output.h"
#include "strategy.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <QApplication>

// #variables (global)
ENVIRONMENT env;
CAR car;
STATION station;
int TIME = 0;

extern GLOB global;

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
    // printLines();
    // ============test
    car.target[0][6] = 1;
    car.target[1][6] = 1;
}

/**按照自然时间的变化，每过一秒将TIME+1
 */
void timeControl(void)
{
    static int lastTIME = 0;
    struct timeval tp;
    gettimeofday(&tp, NULL);
    if (lastTIME == 0 && TIME == 0) {
        lastTIME = tp.tv_sec;
    } else if (lastTIME != tp.tv_sec) {
        lastTIME = tp.tv_sec;
        TIME++;
    }
}

/**当输入检测到end之后，mainLoop会结束
 */
void mainLoop()
{
    if (global.startGame == true) {
        if (env.STRATEGY == ENVIRONMENT::FCFS) {
            // FCFS_readOrder();
        } else {
            // readOrder();
        }
        strategy();
        // outPut();
        timeControl();
    }
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
