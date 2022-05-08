#ifndef MAIN_H
#define MAIN_H

typedef struct{
    int TOTAL_STATION;
    int DISTANCE;
    enum {FCFS, SSTF, SCAN} STRATEGY;
} ENVIRONMENT;

typedef struct{
    int position;
    int target;
} CAR;

typedef struct{
    int clockwise;
    int counterclockwise;
} STATION;

#endif

