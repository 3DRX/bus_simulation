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
int SSTFfindNearestStationNumber( void );

// 寻找在当前行驶方向上最近的车站
// 返回到达用时最短的站台编号
// 如果无请求返回-1

int SCANfindNearestStationNumber( int state );

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

//返回某一方向的距离
int SCAN_stationDistance ( int stationPosition, int state);

void FCFS_haveOnStationRequest(NODE* presentWorkingPtr);

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
    /*printf("=============================\n");*/
    // 状态变量，初始化为STOP
    static enum { STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    if ( state == STOP ) {
        finishRequest(getStationNumber(car.position));//停车状态直接完成本站请求
        if ( s_dest_stationNumber == -1 ) {
            // 如果上一个目标站请求完成，寻找找新的目标站
            s_dest_stationNumber = SSTFfindNearestStationNumber();
        }
        // 确定行驶方向
        if ( s_dest_stationNumber == -1 ) { // 如果当前没有请求，什么也不做
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber) ) == 1 ) {
            state = CLOCKWISE;
            carClockwise();
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber )) == 2 ) {
            state = COUNTERCLOCKWISE;
            carCounterClockwise();
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            /*printf("到站了\n");*/
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            /*printf("途径站点\n");*/
            state = STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            /*printf("到站了\n");*/
            state = STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            /*printf("途径站点\n");*/
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
    // 重置数组第二行
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        car.target[1][i] = 0;
        station.clockwise[1][i] = 0;
        station.counterclockwise[1][i] = 0;
    }
    // start DBG
    /*if ( state == STOP ) {*/
        /*printf( "STOP\n" );*/
    /*}*/
    /*else if ( state == CLOCKWISE ) {*/
        /*printf( "CLOCKWISE\n" );*/
    /*}*/
    /*else if ( state == COUNTERCLOCKWISE ) {*/
        /*printf( "COUNTERCLOCKWISE\n" );*/
    /*}*/
    /*printf("s_dest_stationNumber: %d\n",s_dest_stationNumber);*/
    /*printf("=============================\n");*/
}

void modeFCFS( void )
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
            if ( car.position == dest_positionIndex ) /*停车状态新请求为本站请求，
            直接跳过所有本站请求,如果后续有其他请求则直接开始执行，
            若无其他请求则仍回到NO_TASK状态*/
            {
                FCFS_finishRequest( presentWorkingPtr->where,
                                        presentWorkingPtr->stationNumber );//
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
                if (presentWorkingPtr->next)//如果后续有其他非本站节点则开始执行
                {
                    presentWorkingPtr=presentWorkingPtr->next;
                    dest_positionIndex =getPositionIndex(presentWorkingPtr->stationNumber);
                    if ( orient( dest_positionIndex ) == 1 ) {
                        carClockwise();
                    }
                    else {
                        carCounterClockwise();
                    }
                }
                else/*新的一秒中所有请求都为停车位置本站请求，视为瞬间全部完成，
                状态保持为NO_TASK*/
                {
                    state=NO_TASK;
                }
            }
            else {
                //FCFS_haveOnStationRequest(presentWorkingPtr);
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
                    FCFS_finishRequest( presentWorkingPtr->where, presentWorkingPtr->stationNumber );
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
    //NODE* temp = env.headnode->next;
    // while (temp) {
    // printf("======\n");
    // printf("where:%d\n", temp->where);
    // printf("stationNumber:%d\n", temp->stationNumber);
    // printf("======\n");
    // temp = temp->next;
    //}
}

void modeSCAN( void )
{
    /*SCAN策略相对于SSTF仅改变状态转移函数
    */
    static enum {STOP, CLOCKWISE_STOP, COUNTERCLOCKWISE_STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    if ( state == STOP ){
        s_dest_stationNumber = SSTFfindNearestStationNumber();//不用于确定方向，仅用于确定是否有请求
        if( s_dest_stationNumber==-1){
            state = STOP;
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber) ) == 1 ) {
            state = CLOCKWISE;
            carClockwise();
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber )) == 2 ) {
            state = COUNTERCLOCKWISE;
            carCounterClockwise();
        }
    }
    else if ( state == CLOCKWISE_STOP || state == COUNTERCLOCKWISE_STOP ) {
        finishRequest(getStationNumber(car.position));//停车状态直接完成本站请求
        if ( s_dest_stationNumber == -1 ) {
            // 如果上一个目标站请求完成，寻找找新的目标站
            s_dest_stationNumber = SSTFfindNearestStationNumber();//不用于确定方向，仅用于确定是否有请求
        }
        // 确定行驶方向
        if ( s_dest_stationNumber == -1 ) { // 如果当前没有请求，什么也不做
            state = STOP;
        }
        else if ( state == CLOCKWISE_STOP ) {//停车前行驶方向为顺时针
            int nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 1 )), 1);
            /*printf("====================\n");*/
            /*printf("%d\n===================\n",nearsestDistence);*/
            if (nearsestDistence*2>env.TOTAL_STATION*env.DISTANCE){//按当前行驶方向最近的站点路程大于总长度一半
                state=COUNTERCLOCKWISE;
                carCounterClockwise();
            }
            else{
                state=CLOCKWISE;
                carClockwise();
            }
        }
        else if ( state == COUNTERCLOCKWISE_STOP ) {//停车前行驶方向为逆时针
            /*printf("====================\n");*/
            int nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 2 )), 2);
            /*printf("%d\n===================\n",nearsestDistence);*/
            if (nearsestDistence*2>env.TOTAL_STATION*env.DISTANCE){//按当前行驶方向最近的站点路程大于总长度一半
                state=CLOCKWISE;
                carClockwise();
            }
            else{
                state=COUNTERCLOCKWISE;
                carCounterClockwise();
            }
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            //printf("到站了\n");
            state = CLOCKWISE_STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if (( haveRequest( CLOCKWISE )== TRUE) || (haveRequest( COUNTERCLOCKWISE ) == TRUE )) {
            // SCAN策略下无视请求类型，如果途径站点有请求则停车完成
            //printf("途径站点\n");
            //printf("haveRequest: %d, %d\n", haveRequest(CLOCKWISE), haveRequest(COUNTERCLOCKWISE));
            state = CLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            //printf("到站了\n");
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( s_dest_stationNumber ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if (( haveRequest( CLOCKWISE )== TRUE) || (haveRequest( COUNTERCLOCKWISE ) == TRUE )) {
            // SCAN策略下无视请求类型，如果途径站点有请求则停车完成
            //printf("途径站点\n");
            //printf("==========================\n");
            //printf("haveRequest: %d, %d\n", haveRequest(CLOCKWISE), haveRequest(COUNTERCLOCKWISE));
            //printf("==========================\n");
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) );
        }
        else {
            carCounterClockwise();
        }
    }
    else {
        printf("fkme\n");
    }
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        car.target[1][i] = 0;
        station.clockwise[1][i] = 0;
        station.counterclockwise[1][i] = 0;
    }
}

//------------------------内部函数实现{{{

int SSTFfindNearestStationNumber( void )
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

int SCANfindNearestStationNumber( int state )
{
    int   res = -1;
    int   minDistance = env.DISTANCE * env.TOTAL_STATION;
    int i = 0;
    // 遍历 car.target 中的所有请求
    while ( car.target[ 0 ][ i ] != -1 ) {
        if ( car.target[ 0 ][ i ] == 1 ) {
            if ( minDistance > SCAN_stationDistance( getPositionIndex(i + 1) , state) ) {
                minDistance = SCAN_stationDistance( getPositionIndex(i + 1) , state);
                res = i + 1;
            }
        }
        i++;
    }
    i = 0;
    // 遍历 station.clockwise 中的所有请求
    while ( station.clockwise[ 0 ][ i ] != -1 ) {
        if ( station.clockwise[ 0 ][ i ] == 1 ) {
            if ( minDistance > SCAN_stationDistance( getPositionIndex(i + 1) , state) ) {
                minDistance = SCAN_stationDistance( getPositionIndex(i + 1) , state);
                res = i + 1;
            }
        }
        i++;
    }
    i = 0;
    // 遍历 station.counterclockwise 中的所有请求
    while ( station.counterclockwise[ 0 ][ i ] != -1 ) {
        if ( station.counterclockwise[ 0 ][ i ] == 1 ) {
            if ( minDistance > SCAN_stationDistance( getPositionIndex(i + 1) , state) ) {
                minDistance = SCAN_stationDistance( getPositionIndex(i + 1) , state);
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
    /*else {*/
        /*printf("!!! target error\n");*/
    /*}*/
    if ( station.clockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    /*else {*/
        /*printf("!!! clockwise error\n");*/
    /*}*/
    if ( station.counterclockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
        station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
    }
    /*else {*/
        /*printf("!!! counterclockwise error\n");*/
    /*}*/
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
            if (car.target[1][temp-1] == 1) { // 如果该请求是这一秒新增的，则忽略
            }
            else { // 不是这一秒新增的请求，不忽略
                //printf("进else了\n");
                //printf("%d\n",car.target[1][temp - 1]);
                return TRUE;
            }
        }
        if ( direction == 1 ) // 顺时针
        {
            if ( station.clockwise[ 0 ][ temp - 1 ] == 1 ) {
                if (station.clockwise[1][temp-1] == 1) { // 如果该请求是这一秒新增的，则忽略
                    return FALSE;
                }
                else { // 不是这一秒新增的请求，不忽略
                    return TRUE;
                }
            }
            else {
                return FALSE;
            }
        }
        else // 逆时针
        {
            if ( station.counterclockwise[ 0 ][ temp - 1 ] == 1 ) {
                if (station.counterclockwise[1][temp-1] == 1) { // 如果该请求是这一秒新增的，则忽略
                    return FALSE;
                }
                else { // 不是这一秒新增的请求，不忽略
                    return TRUE;
                }
            }
            else {
                return FALSE;
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

int SCAN_stationDistance( int stationPosition ,int state )//state定义 CLOCKWISE == 1, COUNTERCLOCKWISE ==2
{
    int clockwiseDistence;
    int counterclockwiseDistence;
    if ( car.position > stationPosition ) {
        stationPosition += env.TOTAL_STATION * env.DISTANCE;
    }
    clockwiseDistence = abs( stationPosition - car.position );
    counterclockwiseDistence = env.TOTAL_STATION * env.DISTANCE - clockwiseDistence;
    if ( state == 1 ) {
        return clockwiseDistence;
    }
    else {
        return counterclockwiseDistence;
    }
}

void FCFS_haveOnStationRequest(NODE* presentWorkingPtr)
{
    while (presentWorkingPtr->next)//当前节点不是尾节点
    {
        if (getPositionIndex(presentWorkingPtr->next->stationNumber) == car.position)
        {
            FCFS_finishRequest(presentWorkingPtr->next->where,presentWorkingPtr->next->stationNumber);
            NODE * freeMe = presentWorkingPtr->next;
            if (presentWorkingPtr->next->next){
                presentWorkingPtr->next->next->prev = presentWorkingPtr;
                presentWorkingPtr->next= presentWorkingPtr->next->next;
            }
            else {
                presentWorkingPtr->next = NULL;
                env.presentPtr=presentWorkingPtr;
            }
            free(freeMe);
        }
        else 
        {
            presentWorkingPtr=presentWorkingPtr->next;
        }
    }
}

//------------------------}}}内部函数实现
