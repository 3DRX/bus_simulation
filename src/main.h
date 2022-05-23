#ifndef MAIN_H
#define MAIN_H

#define TRUE ( 1 )
#define FALSE ( 0 )
#define END ( -1 )
#define DBG (-2)

typedef struct {
    int TOTAL_STATION;
    int DISTANCE;
    enum { FCFS, SSTF, SCAN } STRATEGY;
    struct Node * headnode;
    struct Node * presentPtr;
} ENVIRONMENT;

typedef struct {
    int position;
    int target[ 2 ][ 11 ];
} CAR;

typedef struct {
    int clockwise[ 2 ][ 11 ];
    int counterclockwise[ 2 ][ 11 ];
} STATION;

typedef struct Node{
    struct Node *prev; // always NULL in headnode
    int where; // -1 not FCFS; 1 car.target; 2 clockwise; 3 counterclockwise
    int stationNumber; // -1 not FCFS
    struct Node *next;
} NODE;

#endif

