#include "main.h"
#include <stdio.h>
#include <stdlib.h>

extern int         TIME;
extern CAR         car;
extern ENVIRONMENT env;
extern STATION     station;

void printLines( void )
{
    printf( "TIME:%d\n", TIME );
    printf( "BUS:\n" );
    printf( "position:%d\n", car.position );
    car.target[ 0 ][ 10 ] = -1; // 防越界
    printf( "target:" );
    for ( int i = 0; i < 10; i++ ) {
        if ( car.target[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", car.target[ 0 ][ i ] );
    }
    printf( "\n" );
    printf( "STATION:\n" );
    station.clockwise[ 0 ][ 10 ] = -1;        // 防越界
    station.counterclockwise[ 0 ][ 10 ] = -1; // 防越界
    printf( "clockwise:" );
    for ( int i = 0; i < 10; i++ ) {
        if ( station.clockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.clockwise[ 0 ][ i ] );
    }
    printf( "\n" );
    printf( "counterclockwise:" );
    for ( int i = 0; i < 10; i++ ) {
        if ( station.counterclockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.counterclockwise[ 0 ][ i ] );
    }
    printf( "\n" );
}

void updateBuf(void){
    // 疑问：这里需不需要把链表中没有提到的位置都清零？
    NODE * Nptr = env.headnode->next;
    while (Nptr) {
        if (Nptr->where == 1) {
            car.target[0][Nptr->stationNumber-1] = 1;
        }
        else if (Nptr->where == 2) {
            station.clockwise[0][Nptr->stationNumber-1] = 1;
        }
        else if (Nptr->where == 3) {
            station.counterclockwise[0][Nptr->stationNumber-1] = 1;
        }
        Nptr = Nptr->next;
    }
}

void outPut( int ifOutPut )
{
    // 判断是否是FCFS模式，如果是，则updateBuf
    if (env.STRATEGY == FCFS) {
        updateBuf();
    }
    // 判断是否是新的一秒，只有在新的一秒的时候
    // （即一秒之内的所有指令全部写入结构体），
    // 才输出
    static int lastTime = 0;
    if ( lastTime == TIME && TIME != 0 ) {
        return;
    }
    else {
        lastTime = TIME;
        if ( TIME == -1 ) {
            printf( "end\n" );
            exit( EXIT_SUCCESS );
        }
        if ( ifOutPut == TRUE ) {
            printLines();
        }
        else if ( ifOutPut == DBG ) {
            printLines();
            printf("###--debug info:\n");
            printf( "target [1] : " );
            for ( int i = 0; i < env.TOTAL_STATION + 1; i++ ) {
                printf( "%d", car.target[ 1 ][ i ] );
            }
            printf( "\n" );
            printf( "clockwise [1] : " );
            for ( int i = 0; i < env.TOTAL_STATION + 1; i++ ) {
                printf( "%d", station.clockwise[ 1 ][ i ] );
            }
            printf( "\n" );
            printf( "counterclockwise [1] : " );
            for ( int i = 0; i < env.TOTAL_STATION + 1; i++ ) {
                printf( "%d", station.counterclockwise[ 1 ][ i ] );
            }
            printf( "\n" );
            printf( "-----------------------------------------------\n" );
        }
    }
}

