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

typedef struct DigitalOrder{
    struct DigitalOrder * before;
    struct DigitalOrder * next;
} digitalOrder;

typedef struct Order{
    struct aorder * before;
    struct aorder * next;
} order;

// TODO: 这里和文档的不一样，确定好不改了之后把文档里的数据结构定义换成此处的

#endif

