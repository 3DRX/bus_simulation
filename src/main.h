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
    enum { FCFS,
           SSTF,
           SCAN } STRATEGY;
    struct Node* headnode;
    struct Node* presentPtr;
    struct Node* presentWorkingPtr;
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
    struct Node* prev;          // always NULL in headnode
    int          where;         // -1 not FCFS; 1 car.target; 2 clockwise; 3 counterclockwise
    int          stationNumber; // -1 not FCFS
    struct Node* next;
} NODE;

// 用于线程之间共享信息
typedef struct {
    //用于指示fcfs模式下当前指向的节点任务是否完成
    int ifFCFSRequestFinished;

    // 车的位置（角度theta）
    int car_theta;
    enum { CLOCKWISE,
           COUNTERCLOCKWISE,
           STOP } car_state;

    // 由车到每一个小站点触发，
    // strategy模块依靠这个判断是否更新策略
    bool glob_state_refresh;

    // 控制游戏的暂停、继续
    bool startGame;

    // 控制游戏是否结束
    // （是否释放内存，结束进程）
    bool terminate;
} GLOB;

#endif
