#include "main.h"
#include "control.h"
#include "input.h"
#include "output.h"
#include "strategy.h"
#include <stdio.h>
#include <stdlib.h>

// #variables (global)
ENVIRONMENT env;
CAR         car;
STATION     station;
int         TIME = 0;
// TODO: 这三个将来可以放进main函数

// #functions

/*
 * 初始化：
 * 配置参数
 * car position
 * car target
 * station clockwise & counterclockwise
 */
void initGame( void )
{
    // init environment
    FILE* fPtr = NULL;
    if ( ( fPtr = fopen( "dict.dic", "r" ) ) == NULL ) {
        printf( "Can't open file \"dict.dic\"\n" );
        exit( EXIT_FAILURE );
    }
    readfile( fPtr );
    // init car & station
    // 使用 env.TOTAL_STATION 令输出长度可变（如果规则要求输出长度永远是10
    // 就把本函数中所有的 env.TOTAL_STATION 替换为10）
    car.position = 0;
    for ( int i = 0; i < 20; i++ ) {
        car.target[ 0 ][ i ] = 0;
        station.clockwise[ 0 ][ i ] = 0;
        station.counterclockwise[ 0 ][ i ] = 0;
        // 将数组的第二行全部置0
        car.target[ 1 ][ i ] = 0;
        station.clockwise[ 1 ][ i ] = 0;
        station.counterclockwise[ 1 ][ i ] = 0;
    }
    car.target[ 0 ][ env.TOTAL_STATION] = -1;
    station.clockwise[ 0 ][ env.TOTAL_STATION] = -1;
    station.counterclockwise[ 0 ][ env.TOTAL_STATION] = -1;
    // 上面三行中 -1 表示行结束（类似'\0'）
    // 如果是FCFS模式，初始化链表
    if ( env.STRATEGY == FCFS ) {
        env.headnode = malloc( sizeof( NODE ) );
        env.headnode->prev = NULL;
        env.headnode->next = NULL;
        env.headnode->stationNumber = -1;
        env.headnode->where = -1;
        env.presentPtr = env.headnode;
    }
    else {
        env.headnode = NULL;
    }
    // 初始化env.presentWorkingPtr
    env.presentWorkingPtr = NULL;
    // 输出TIME: 0
    printLines();
}

void mainLoop( void )
{
    if (env.STRATEGY == FCFS) {
        FCFS_readOrder();
    }
    else {
        readOrder();
    }
    strategy();
    outPut();
}

// #main
int main( void )
{
    initGame();
    while ( 1 ) { // main loop
        mainLoop();
    }
    return 0;
}

