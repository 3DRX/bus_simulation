#include "main.h"
#include <stdio.h>

extern int     TIME;
extern CAR     car;
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
    if ( ifOutPut == TRUE ) {
        printLines();
    }
    else if ( ifOutPut == END ) {
        printf( "end\n" );
    }
}

