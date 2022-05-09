#ifndef MAIN_H
#define MAIN_H

typedef struct{
    int TOTAL_STATION;
    int DISTANCE;
    enum {FCFS, SSTF, SCAN} STRATEGY;
} ENVIRONMENT;

typedef struct{
    int position;
    int target[10];
} CAR;

typedef struct{
    int clockwise[10];
    int counterclockwise[10];
} STATION;

typedef struct{

} digitalOrder;

#endif

