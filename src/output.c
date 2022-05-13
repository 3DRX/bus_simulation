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
    printf("target: %s\n",car.target); // TODO print target sequence
    printf("STATION:\n");
    station.clockwise[10] = '\0'; // 防越界
    station.counterclockwise[10] = '\0'; // 防越界
    printf("clockwise: %s\n",station.clockwise); // TODO print clockwise sequence
    printf("counterclockwise: %s\n",station.counterclockwise); // TODO print counterclockwise sequence
}

void output(int ifOutPut)
{
    /*
     * 参数：ifOutPut
     * 为TRUE则输出
     * 否则不输出
     */
    if (ifOutPut == TRUE) {
        printLines();
    }
}

