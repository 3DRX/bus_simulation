#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
void finishRequest(int stationNumber);

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
        printf("do strategy\n");
        if ( env.STRATEGY == FCFS ) {
            modeFCFS();
        }
        else if ( env.STRATEGY == SSTF ) {
            modeSSTF();
        }
        else if ( env.STRATEGY == SCAN ) {
            modeSCAN();
        }
    }
}

void modeSSTF( void )
{
    // 状态变量，初始化为NO_TASK
    static enum { NO_TASK, CLOCKWISE, COUNTERCLOCKWISE, STOP } state = NO_TASK;
    static int s_dest_stationNumber = -1;
    if ( state == NO_TASK ) {
        // 找到距离最近的请求，确定行驶方向
        if ( findNearestStationNumber() == -1 ) { // 如果不存在请求，直接退出函数
            return;
        }
        s_dest_stationNumber = findNearestStationNumber();
        int dest_positionIndex = getPositionIndex( s_dest_stationNumber );
        int fullLength = env.DISTANCE * env.TOTAL_STATION;
        // 判断行驶到最近的请求是什么方向
        if ( abs( dest_positionIndex - car.position ) < fullLength ) { // TODO:这句不一定对
            state = CLOCKWISE;
        }
        else {
            state = COUNTERCLOCKWISE;
        }
    }
    else if ( state == CLOCKWISE ) {
        // TODO
        // 这里实际上是SCAN顺便服务策略
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = STOP;
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        // TODO
        // 这里实际上是SCAN顺便服务策略
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = STOP;
        }
    }
    else if ( state == STOP ) {
        finishRequest(s_dest_stationNumber);
        s_dest_stationNumber = -1;
        state = NO_TASK;
    }
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
    int temp = getStationNumber(stationNumber);
    int temp1 = abs(car.position - temp);
    if(env.TOTAL_STATION>=2*temp1)
    {
        return temp1;
    }
    else
    {
        return env.TOTAL_STATION - temp1;
    }
}

int getStationNumber( int positionIndex )
{
    if (positionIndex % env.DISTANCE == 0)
    {
        int temp = positionIndex / env.DISTANCE;
        return temp + 1;
    }
    else { return -1; }
    // TODO
}

int getPositionIndex( int stationNumber )
{
    int temp = stationNumber - 1;
    return temp * env.DISTANCE;
    // TODO
}

void finishRequest(int stationNumber)
{
    if (car.target[0][stationNumber-1] == 1) {
        car.target[0][stationNumber-1] = 0;
    }
    if (station.clockwise[0][stationNumber-1] == 1) {
        station.clockwise[0][stationNumber-1] = 0;
    }
    if (station.counterclockwise[0][stationNumber-1] == 1) {
        station.counterclockwise[0][stationNumber-1] = 0;
    }
    // TODO: 修改数组的第二行
}

//------------------------}}}内部函数实现
