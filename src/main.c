#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "input.h"
#include "output.h"
#include "strategy.h"
#include "control.h"

// #variables (global)
ENVIRONMENT env;
CAR car;
STATION station;
int TIME = 0;
// TODO: 这三个将来可以放进main函数

// #functions
void initGame(void)
{
    /*
     * 初始化：
     * 配置参数
     * car position
     * car target
     * station clockwise & counterclockwise
     */
    // init environment
    FILE* fPtr = NULL;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) {
        printf("Can't open file \"dict.dic\"\n");
        exit(EXIT_FAILURE);
    }
    readfile(fPtr);
    // init car & station
    // 使用 env.TOTAL_STATION 令输出长度可变（如果规则要求输出长度永远是10
    // 就把所有的 env.TOTAL_STATION 替换为10）
    car.position = 0;
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        car.target[i] = '0';
        station.clockwise[i] = '0';
        station.counterclockwise[i] = '0';
    }
    car.target[env.TOTAL_STATION+1] = '\0';
    station.clockwise[env.TOTAL_STATION+1] = '\0';
    station.counterclockwise[env.TOTAL_STATION+1] = '\0';
}

void mainLoop(void)
{
    // output
    // 输出应在每一个 mainLoop 的开头进行
    // test
    short penis = FALSE;
    if (getchar() == 'a') {
        penis = TRUE;
    }
    outPut(penis);
    // endtest
}

// #main
int main(void)
{
    initGame();
    while (1) { // main loop
        mainLoop();
    }
    return 0;
}

