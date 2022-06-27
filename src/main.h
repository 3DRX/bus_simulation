#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <sys/time.h>

#define TRUE (1)
#define FALSE (0)
#define END (-1)
#define DBG (-2)

typedef struct {
    int TOTAL_STATION;
    int DISTANCE;
    enum { FCFS, SSTF, SCAN } STRATEGY;
    struct Node *headnode;
    struct Node *presentPtr;
    struct Node *presentWorkingPtr;
    FILE *input;
    FILE *output;

} ENVIRONMENT;

// 数组第二行定义：
// 为0则代表不是这一秒的新增请求
// 为1代表是这一秒的新增请求
typedef struct {
    int position;
    int target[2][21];
} CAR;

typedef struct {
    int clockwise[2][21];
    int counterclockwise[2][21];
} STATION;

typedef struct Node {
    struct Node *prev; // always NULL in headnode
    int where; // -1 not FCFS; 1 car.target; 2 clockwise; 3 counterclockwise
    int stationNumber; // -1 not FCFS
    struct Node *next;
} NODE;

// 用于线程之间共享信息
typedef struct {
    // 大多是时候是true，只有当next按钮按下之后变为false
    // output模块中检测的值为true之后调用一次printLines()函数，
    // 之后将值重置为true
    bool ifWait;

    // 车的位置（角度theta）
    int car_theta;
    enum { CLOCKWISE, COUNTERCLOCKWISE, STOP } car_state;

    // 由车到每一个小站点触发，
    // strategy模块依靠这个判断是否更新策略
    bool glob_state_refresh;
} GLOB;

#endif
