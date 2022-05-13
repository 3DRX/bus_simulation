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
// TODO: 这三个将来可以放进main函数

// #functions
void initGame(void)
{
    // init environment
    FILE* fPtr = NULL;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) {
        printf("Can't open file \"dict.dic\"\n");
        exit(EXIT_FAILURE);
    }
    readfile(fPtr);
    // init car
    car.position = 0;
}

// #main
int main(void)
{
    int TIME = 0;
    order * headNode;
    initGame();
    // test
    printf("TOTAL_STATION:%d\n",env.TOTAL_STATION);
    if (env.STRATEGY == FCFS) {
        printf("STRATEGY: FCFS\n");
    }
    else if (env.STRATEGY == SSTF) {
        printf("STRATEGY: SSTF\n");
    }
    else if (env.STRATEGY == SCAN) {
        printf("STRATEGY: SCAN\n");
    }
    printf("DISTANCE:%d\n",env.DISTANCE);
    // endtest

    while (1) { // main loop
    }
    return 0;
}

