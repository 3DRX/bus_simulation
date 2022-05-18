#include "main.h"
#include <stdio.h>
#include <stdlib.h>

extern int     TIME;
extern CAR     car;
extern ENVIRONMENT     env;
extern STATION station;

void printLines( void )
{
    printf( "TIME:%d\n", TIME );
    printf( "BUS:\n" );
    printf( "position:%d\n", car.position );
    car.target[ 0 ][ 10 ] = -1; // 防越界
    printf( "target: " );
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
    printf( "clockwise: " );
    for ( int i = 0; i < 10; i++ ) {
        if ( station.clockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.clockwise[ 0 ][ i ] );
    }
    printf( "\n" );
    printf( "counterclockwise: " );
    for ( int i = 0; i < 10; i++ ) {
        if ( station.counterclockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.counterclockwise[ 0 ][ i ] );
    }
    printf( "\n" );
}

void outPut( int ifOutPut )
{
    if (TIME == -1) {
        printf("end\n");
        exit(EXIT_SUCCESS);
    }
    if ( ifOutPut == TRUE ) {
        printLines();
    }
    else if ( ifOutPut == END ) {
        printf( "end\n" );
    }
    else if (ifOutPut == DBG) {
        printLines();
        printf("target [1] : ");
        for (int i = 0; i < env.TOTAL_STATION + 1; i++) {
            printf("%d",car.target[1][i]);
        }
        printf("\n");
        printf("clockwise [1] : ");
        for (int i = 0; i < env.TOTAL_STATION + 1; i++) {
            printf("%d",station.clockwise[1][i]);
        }
        printf("\n");
        printf("counterclockwise [1] : ");
        for (int i = 0; i < env.TOTAL_STATION + 1; i++) {
            printf("%d",station.counterclockwise[1][i]);
        }
        printf("\n");
    }
}

