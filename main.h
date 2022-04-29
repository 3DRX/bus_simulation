#ifndef MAIN_H
#define MAIN_H

typedef struct{
    int TOTAL_STATION;
    int DISTANCE;
    enum {FCFS, SSTF, SCAN} STRATEGY;
} ENVIRONMENT;

typedef struct{
    int CURRENT_STATION;
} CAR;

#endif

