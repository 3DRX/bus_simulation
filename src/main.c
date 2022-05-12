#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "input.h"
#include "output.h"
#include "strategy.h"
#include "control.h"

// #variables (global)
ENVIRONMENT * env;
CAR * car;
STATION * station;
// TODO: 这三个将来可以放进main函数

// #functions

void initGame(int TIME)
{
    // init environment
    FILE* fPtr;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) {
        printf("Can't open file \"dict.dic\"\n");
        exit(EXIT_FAILURE);
    }
    readfile(fPtr);
    // init car
    car->position = 0;
    //init time
    TIME = 0;
}

// #main

int main(int argc, char**argv)
{
    int TIME;
    order * headNode;
    initGame(TIME);

    while (1) { // main loop
    }
    return 0;
}

