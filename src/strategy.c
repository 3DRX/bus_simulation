#include "main.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

extern ENVIRONMENT env;
extern CAR         car;
extern STATION     station;
extern int         TIME;

void modeFCFS();
void modeSSTF();
void modeSCAN();

//------------------------内部函数声明{{{

/**寻找当前所有请求中最短的那个，
 * 返回到达用时最短的站台编号（从1开始）。
 * 如果无请求，就返回-1
 */
int findNearestStationNumber( void );

/**计算当前位置与目标位置之间的距离。
 * stationNumber：目标站号（从1开始）
 * 返回positionIndex
 */
int stationDistance( int stationNumber );

/**通过positionIndex计算对应的stationNumber，
 * 如果position在站点上，返回站台编号，
 * 如果所在position不是站点，返回-1。
 */
int getStationNumber( int positionIndex );

/**通过stationNumber计算对应的positionIndex，
 */
int getPositionIndex( int stationNumber );

/**完成输入stationNumber的请求
 * （把数组中的1改成0）
 */
void finishRequest( int stationNumber );

/**车辆顺时针移动一个单位长度
 */
void carClockwise( void );

/**车辆拟时针移动一个单位长度
 */
void carCounterClockwise( void );

/**检测car当前的位置有没有请求
 * direction: 1 顺时针，2 逆时针
 * 如果有，返回TRUE
 * 如果没有，返回FALSE
 * 如果当前位置不是车站，返回-1
 */
short haveRequest( short direction );

//------------------------}}}内部函数声明

void strategy( void )
{
    // 判断是否是新的一秒，只有在新的一秒的时候
    // （即一秒之内的所有指令全部写入结构体），
    // 才进行strategy的判断
    static int lastTime = 0;
    if ( lastTime == TIME ) {
        return;
    }
    else {
        lastTime = TIME;
        if ( env.STRATEGY == FCFS ) {
            modeFCFS();
        }
        else if ( env.STRATEGY == SSTF ) {
            // printf( "###--debug info:\n" );
            modeSSTF();
            // printf( "-----------------------------------------------\n" );
        }
        else if ( env.STRATEGY == SCAN ) {
            modeSCAN();
        }
    }
}

void modeSSTF( void )
{
    // 状态变量，初始化为STOP
    static enum { STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    if ( state == STOP ) {
        if ( s_dest_stationNumber == -1 ) {
            // 如果上一个目标站请求完成，寻找找新的目标站
            s_dest_stationNumber = findNearestStationNumber();
        }
        // 确定行驶方向
        if ( s_dest_stationNumber == -1 ) { // 如果当前没有请求，什么也不做
        }
        else if ( abs( getPositionIndex( s_dest_stationNumber ) - car.position ) * 2
                  < env.DISTANCE * env.TOTAL_STATION ) { // 这里要不要<=?
            state = CLOCKWISE;
            carClockwise();
        }
        else {
            state = COUNTERCLOCKWISE;
            carCounterClockwise();
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carCounterClockwise();
        }
    }
    // printf( "state: " );
    // if ( state == NO_TASK ) {
    // printf( "NO_TASK\n" );
    //}
    // else if ( state == CLOCKWISE ) {
    // printf( "CLOCKWISE\n" );
    //}
    // else if ( state == COUNTERCLOCKWISE ) {
    // printf( "COUNTERCLOCKWISE\n" );
    //}
}

void modeFCFS()
{
    // TODO
}

void modeSCAN()
{
    // TODO
}

//------------------------内部函数实现{{{

int findNearestStationNumber( void )
{
    int   res = -1;
    int   minDistance = 10;
    short i = 0;
    // 遍历 car.target 中的所有请求
    while ( car.target[ 0 ][ i ] != -1 ) {
        if ( car.target[ 0 ][ i ] == 1 ) {
            if ( minDistance > stationDistance( i + 1 ) ) {
                minDistance = stationDistance( i + 1 );
                res = i + 1;
            }
        }
        i++;
    }
    i = 0;
    // 遍历 station.clockwise 中的所有请求
    while ( station.clockwise[ 0 ][ i ] != -1 ) {
        if ( station.clockwise[ 0 ][ i ] == 1 ) {
            if ( minDistance > stationDistance( i + 1 ) ) {
                minDistance = stationDistance( i + 1 );
                res = i + 1;
            }
        }
        i++;
    }
    i = 0;
    // 遍历 station.counterclockwise 中的所有请求
    while ( station.counterclockwise[ 0 ][ i ] != -1 ) {
        if ( station.counterclockwise[ 0 ][ i ] == 1 ) {
            if ( minDistance > stationDistance( i + 1 ) ) {
                minDistance = stationDistance( i + 1 );
                res = i + 1;
            }
        }
        i++;
    }
    return res;
}

int stationDistance( int stationNumber )
{
    int temp = getPositionIndex( stationNumber );
    int temp1 = abs( car.position - temp );
    if ( env.TOTAL_STATION * env.DISTANCE >= 2 * temp1 ) {
        return temp1;
    }
    else {
        return env.TOTAL_STATION * env.DISTANCE - temp1;
    }
}

int getStationNumber( int positionIndex )
{
    if ( positionIndex % env.DISTANCE == 0 ) {
        int temp = positionIndex / env.DISTANCE;
        return temp + 1;
    }
    else {
        return -1;
    }
}

int getPositionIndex( int stationNumber )
{
    int temp = stationNumber - 1;
    return temp * env.DISTANCE;
}

void finishRequest( int stationNumber )
{
    if ( car.target[ 0 ][ stationNumber - 1 ] == 1 ) {
        car.target[ 0 ][ stationNumber - 1 ] = 0;
    }
    if ( station.clockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    if ( station.counterclockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    // TODO: 修改数组的第二行
}

void carClockwise()
{
    if ( car.position != env.DISTANCE * env.TOTAL_STATION - 1 ) {
        car.position++;
    }
    else {
        car.position = 0;
    }
}

void carCounterClockwise()
{
    if ( car.position != 0 ) {
        car.position--;
    }
    else {
        car.position = env.DISTANCE * env.TOTAL_STATION - 1;
    }
}

short haveRequest( short direction )
{
    int temp = getStationNumber(car.position);
    if (temp == -1) 
    { 
        return -1;
    }
    else
    {
        if (car.target[0][temp] == 1)//判断车上是否有请求
        {
            return TRUE;
        }
        else
        {
            if (direction == 1)//顺时针
            {
                if (station.clockwise[0][temp] == 1)
                { 
                    return TRUE;
                }
                else 
                { 
                    return FALSE;
                }
            }
            else if (direction == 2)//逆时针
            {
                if (station.counterclockwise[0][temp] == 1)
                { 
                    return TRUE;
                }
                else 
                {
                    return FALSE;
                }
            }
        }
    }
    

   
    // TODO
}

//------------------------}}}内部函数实现
