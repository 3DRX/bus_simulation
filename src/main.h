#ifndef MAIN_H
#define MAIN_H

#define TRUE ( 1 )
#define FALSE ( 0 )
#define END ( -1 )

typedef struct {
    int TOTAL_STATION;
    int DISTANCE;
    enum { FCFS, SSTF, SCAN } STRATEGY;
} ENVIRONMENT;

typedef struct {
    int  position;
    char target[ 11 ];
} CAR;

typedef struct {
    char clockwise[ 11 ];
    char counterclockwise[ 11 ];
} STATION;

// DISCUSSTION:
// 有必要将order单独作为结构体存储吗？
// 既然策略的判断都不依赖后面的指令，
// 是否可以在读入请求后直接存在 CAR 和 STATION
// 的 target 和 clock/counterclockwise 中（怎么输出就怎么存）？
typedef struct DigitalOrder {
    struct DigitalOrder* before;
    struct DigitalOrder* next;
} digitalOrder;

typedef struct Order {
    struct Order* before;
    struct Order* next;
} order;

// TODO: 这里和文档的不一样，确定好不改了之后
// 把文档里的数据结构定义换成此处的。

#endif

