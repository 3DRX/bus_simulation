#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "readfile.h"

// #variables (global)
ENVIRONMENT * env;
CAR * car;
STATION * station;

// #functions

void initGame(void)
{
    // init environment
    FILE* fPtr= fopen("dict.dic", "r");
    readfile(fPtr);
    // init car
    car->position = 0;
}

// #main

int main(int argc, char**argv)
{
    initGame();
    return 0;
}

