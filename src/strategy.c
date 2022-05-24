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

/**用于FCFS策略中完成请求时从全局变量environment和car中去除对应请求
 */
void FCFS_finishRequest( int where, int stationNumber );

/*用于计算车前往当前目标站点采用的方向,顺时针返回1，逆时针返回2.
 */
int orient( int stationPosition );

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
            modeSSTF();
        }
        else if ( env.STRATEGY == SCAN ) {
            modeSCAN();
        }
    }
}

void modeSSTF( void )
{
    printf("=============================\n");
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
        else if ( orient( s_dest_stationNumber ) == 1 ) {
            state = CLOCKWISE;
            carClockwise();
        }
        else if ( orient( s_dest_stationNumber ) == 2 ) {
            state = COUNTERCLOCKWISE;
            carCounterClockwise();
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            printf("到站了\n");
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            printf("途径站点\n");
            state = STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            printf("到站了\n");
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            printf("途径站点\n");
            state = STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carCounterClockwise();
        }
    }
    else {
        printf("sth wrong\n");
    }
    if ( state == STOP ) {
        printf( "NO_TASK\n" );
    }
    else if ( state == CLOCKWISE ) {
        printf( "CLOCKWISE\n" );
    }
    else if ( state == COUNTERCLOCKWISE ) {
        printf( "COUNTERCLOCKWISE\n" );
    }
    printf("s_dest_stationNumber: %d\n",s_dest_stationNumber);
    printf("=============================\n");
}

void modeFCFS()
{
    static enum { NO_TASK, WORKING } state = NO_TASK;
    static NODE* presentWorkingPtr = NULL;
    if ( presentWorkingPtr == NULL ) {
        presentWorkingPtr = env.headnode;
    }
    int dest_positionIndex = getPositionIndex( presentWorkingPtr->stationNumber );
    int fullLength = env.DISTANCE * env.TOTAL_STATION;
    if ( state == NO_TASK ) {
        if ( presentWorkingPtr->next ) //说明有新任务
        {
            presentWorkingPtr = presentWorkingPtr->next;
            state = WORKING;
            dest_positionIndex = getPositionIndex( presentWorkingPtr->stationNumber );
            if ( car.position == dest_positionIndex ) //当前请求已完成，判定后续节点请求可否同时完成
            {
                state = NO_TASK; //下一次clock指令调用本模块时再次进入working状态
                while ( presentWorkingPtr->next ) {
                    if ( presentWorkingPtr->next->stationNumber
                         == presentWorkingPtr->stationNumber ) {
                        presentWorkingPtr = presentWorkingPtr->next;
                        FCFS_finishRequest( presentWorkingPtr->where,
                                            presentWorkingPtr->stationNumber );
                    }
                    else {
                        break;
                    }
                    //将指针定位到最后一个与当前请求相同的节点，以上请求视为全部同时完成
                }
            }
            else {
                if ( orient( dest_positionIndex ) == 1 ) {
                    carClockwise();
                }
                else {
                    carCounterClockwise();
                }
            }
            // printf("我不摆了\n");
        }
        else {
            // printf("我接着摆\n");
        }
    }
    else //当前有任务
    {
        if ( car.position == dest_positionIndex ) //当前请求已完成，判定后续节点请求可否同时完成
        {
            state = NO_TASK; //下一次clock指令调用本模块时再次进入working状态
            FCFS_finishRequest( presentWorkingPtr->where, presentWorkingPtr->stationNumber );
            while ( presentWorkingPtr->next ) {
                if ( presentWorkingPtr->next->stationNumber == presentWorkingPtr->stationNumber ) {
                    presentWorkingPtr = presentWorkingPtr->next;
                }
                else {
                    break;
                }
                //将指针定位到最后一个与当前请求相同的节点，以上请求视为全部同时完成
            }
        }
        else {
            if ( orient( dest_positionIndex ) == 1 ) {
                carClockwise();
            }
            else {
                carCounterClockwise();
            }
        }
    }
    NODE* temp = env.headnode->next;
    // while (temp) {
    // printf("======\n");
    // printf("where:%d\n", temp->where);
    // printf("stationNumber:%d\n", temp->stationNumber);
    // printf("======\n");
    // temp = temp->next;
    //}
}

void modeSCAN()
{
    // TODO
}

//------------------------内部函数实现{{{

int findNearestStationNumber( void )
{
    int   res = -1;
    int   minDistance = env.DISTANCE * env.TOTAL_STATION;
    int i = 0;
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
    int stationPosition = getPositionIndex(stationNumber);
    int clockwiseDistence;
    int counterclockwiseDistence;
    if ( car.position > stationPosition ) {
        stationPosition += env.TOTAL_STATION * env.DISTANCE;
    }
    clockwiseDistence = abs( stationPosition - car.position );
    counterclockwiseDistence = env.TOTAL_STATION * env.DISTANCE - clockwiseDistence;
    if ( clockwiseDistence > counterclockwiseDistence ) {
        return counterclockwiseDistence;
    }
    else {
        return clockwiseDistence;
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
    else {
        printf("!!! target error\n");
    }
    if ( station.clockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    else {
        printf("!!! clockwise error\n");
    }
    if ( station.counterclockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    else {
        printf("!!! counterclockwise error\n");
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
    int temp = getStationNumber( car.position );
    if ( temp == -1 ) {
        return -1;
    }
    else {
        if ( car.target[ 0 ][ temp - 1 ] == 1 ) //判断车上是否有请求
        {
            return TRUE;
        }
        else {
            if ( direction == 1 ) //顺时针
            {
                if ( station.clockwise[ 0 ][ temp - 1 ] == 1 ) {
                    return TRUE;
                }
                else {
                    return FALSE;
                }
            }
            else //逆时针
            {
                if ( station.counterclockwise[ 0 ][ temp - 1 ] == 1 ) {
                    return TRUE;
                }
                else {
                    return FALSE;
                }
            }
        }
    }
}

void FCFS_finishRequest( int where, int stationNumber )
{
    if ( where == 1 ) {
        car.target[ 0 ][ stationNumber - 1 ] = 0;
    }
    else if ( where == 2 ) {
        station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    else if ( where == 3 ) {
        station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
}

int orient( int stationPosition )
{
    int clockwiseDistence;
    int counterclockwiseDistence;
    if ( car.position > stationPosition ) {
        stationPosition += env.TOTAL_STATION * env.DISTANCE;
    }
    clockwiseDistence = abs( stationPosition - car.position );
    counterclockwiseDistence = env.TOTAL_STATION * env.DISTANCE - clockwiseDistence;
    if ( clockwiseDistence > counterclockwiseDistence ) {
        return 2;
    }
    else {
        return 1;
    }
}

//------------------------}}}内部函数实现
