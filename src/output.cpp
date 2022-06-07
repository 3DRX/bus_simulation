#include "input.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

extern int         TIME;
extern CAR         car;
extern ENVIRONMENT env;
extern STATION     station;

void printLines( void )
{
    fprintf( env.output,"TIME:%d\n", TIME );
    fprintf(env.output, "BUS:\n" );
    fprintf(env.output, "position:%d\n", car.position );
    car.target[ 0 ][ 20 ] = -1; // 防越界
    fprintf(env.output, "target:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( car.target[ 0 ][ i ] == -1 ) {
            break;
        }
        fprintf(env.output, "%d", car.target[ 0 ][ i ] );
    }
    fprintf(env.output, "\n" );
    fprintf(env.output, "STATION:\n" );
    station.clockwise[ 0 ][ 20 ] = -1;        // 防越界
    station.counterclockwise[ 0 ][ 20 ] = -1; // 防越界
    fprintf(env.output, "clockwise:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( station.clockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        fprintf(env.output, "%d", station.clockwise[ 0 ][ i ] );
    }
    fprintf(env.output, "\n" );
    fprintf(env.output, "counterclockwise:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( station.counterclockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        fprintf(env.output, "%d", station.counterclockwise[ 0 ][ i ] );
    }
    fprintf(env.output, "\n" );
}

void outPut( bool * ifExit )
{
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
            fprintf(env.output, "end\n" );
            if ( env.STRATEGY == ENVIRONMENT::FCFS ) {
                FCFS_freeList( env.headnode );
            }
            *ifExit = true;
            return;
        }
        if ( TIME != 0 ) {
            printLines();
        }
    }
}

