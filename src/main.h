#ifndef MAIN_H
#define MAIN_H

#define TRUE (1)
#define FALSE (0)

typedef struct{
    int TOTAL_STATION;
    int DISTANCE;
    enum {FCFS, SSTF, SCAN} STRATEGY;
} ENVIRONMENT;

typedef struct{
    int position;
    char target[11];
} CAR;

typedef struct{
    char clockwise[11];
    char counterclockwise[11];
} STATION;

typedef struct DigitalOrder{
    struct DigitalOrder * before;
    struct DigitalOrder * next;
} digitalOrder;

typedef struct Order{
    struct Order * before;
    struct Order * next;
} order;

// TODO: 这里和文档的不一样，确定好不改了之后把文档里的数据结构定义换成此处的

#endif

