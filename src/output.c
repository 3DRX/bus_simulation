#include "main.h"
#include <stdio.h>

extern int TIME;
extern CAR car;
extern STATION station;

void printLines(void)
{
    printf("TIME:%d\n",TIME);
    printf("BUS:\n");
    printf("position:%d\n",car.position);
    car.target[10] = '\0'; // 防越界
    printf("target: %s\n",car.target);
    printf("STATION:\n");
    station.clockwise[10] = '\0'; // 防越界
    station.counterclockwise[10] = '\0'; // 防越界
    printf("clockwise: %s\n",station.clockwise);
    printf("counterclockwise: %s\n",station.counterclockwise);
}

void output(int ifOutPut)
{
    /*
     * 参数：ifOutPut
     * 为TRUE则输出
     * 为FALSE不输出
     * 为END输出结束行
     */
    if (ifOutPut == TRUE) {
        printLines();
    }
    else if (ifOutPut == END) {
        printf("end\n");
    }
}

