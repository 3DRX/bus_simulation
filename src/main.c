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
    for ( int i = 0; i < env.TOTAL_STATION; i++ ) {
        car.target[ 0 ][ i ] = 0;
        station.clockwise[ 0 ][ i ] = 0;
        station.counterclockwise[ 0 ][ i ] = 0;
    }
    car.target[ 0 ][ env.TOTAL_STATION + 1 ] = -1;
    station.clockwise[ 0 ][ env.TOTAL_STATION + 1 ] = -1;
    station.counterclockwise[ 0 ][ env.TOTAL_STATION + 1 ] = -1;
    // 上面三行中 -1 表示行结束（类似'\0'）
}

void mainLoop( void )
{
    // 1: output
    // 输出应在每一个 mainLoop 的开头进行
    // test
    short penis = FALSE;
    if ( getchar() == 'a' ) {
        TIME++;
        if ( TIME == 5 ) {
            car.target[ 0 ][ 1 ] = 1;
        }
        if ( TIME == 8 ) {
            car.target[ 0 ][ 1 ] = 0;
        }
        penis = TRUE;
    }
    outPut( penis );
    // endtest

    // 2: input
    // 3: strategy
    // test
    strategy();
    // endtest
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

