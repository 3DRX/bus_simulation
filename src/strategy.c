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

/**完成输入stationNumber的指定类型请求
 * direction：0双向，1顺时针，2逆时针
 * （把数组中的1改成0）
 * ifDelay：TRUE则考虑数组第二行，FALSE则忽略数组第二行
 */
void finishRequest( int stationNumber, int direction, int ifDelay);

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

void updateBuf(NODE* presentPtr );

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
    // 状态变量，初始化为STOP
    static enum { STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    if ( state == STOP ) {
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
            state = STOP;
            finishRequest( s_dest_stationNumber ,0, TRUE); // 完成请求
            s_dest_stationNumber = SSTFfindNearestStationNumber();
            if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 原地请求，立即完成，不改变state
                finishRequest(getStationNumber(car.position), 0, FALSE);
            }
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = STOP;
            finishRequest( getStationNumber( car.position ) ,1, TRUE);
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = STOP;
            finishRequest( s_dest_stationNumber ,0, TRUE); // 完成请求
            s_dest_stationNumber = SSTFfindNearestStationNumber();
            if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 原地请求，立即完成，不改变state
                finishRequest(getStationNumber(car.position), 0, FALSE);
            }
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = STOP;
            finishRequest( getStationNumber( car.position ) ,2, TRUE);
        }
        else {
            carCounterClockwise();
        }
    }
    // 重置数组第二行
    for (int i = 0; i < 20; i++) {
        car.target[1][i] = 0;
        station.clockwise[1][i] = 0;
        station.counterclockwise[1][i] = 0;
    }
}

void modeFCFS( void )
{
    static enum { NO_TASK, WORKING } state = NO_TASK;
    if ( env.presentWorkingPtr == NULL ) {
        env.presentWorkingPtr = env.headnode;
    }
    int dest_positionIndex = getPositionIndex( env.presentWorkingPtr->stationNumber );
    int fullLength = env.DISTANCE * env.TOTAL_STATION;
    if ( state == NO_TASK ) {
        if ( env.presentWorkingPtr->next ) //说明有新任务
        {
            env.presentWorkingPtr = env.presentWorkingPtr->next;
            state = WORKING;
            dest_positionIndex = getPositionIndex( env.presentWorkingPtr->stationNumber );
            if ( car.position == dest_positionIndex ) /*停车状态新请求为本站请求，
            直接跳过所有本站请求,如果后续有其他请求则直接开始执行，
            若无其他请求则仍回到NO_TASK状态*/
            {
                while ( env.presentWorkingPtr->next ) {
                    if ( env.presentWorkingPtr->next->stationNumber
                         == env.presentWorkingPtr->stationNumber ) {
                        env.presentWorkingPtr = env.presentWorkingPtr->next;
                    }
                    else {
                        break;
                    }
                    //将指针定位到最后一个与当前请求相同的节点，以上请求视为全部同时完成
                }
                if (env.presentWorkingPtr->next)//如果后续有其他非本站节点则开始执行
                {
                    updateBuf(env.presentWorkingPtr->next);
                    env.presentWorkingPtr=env.presentWorkingPtr->next;
                    dest_positionIndex =getPositionIndex(env.presentWorkingPtr->stationNumber);
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
                    updateBuf(NULL);
                    state=NO_TASK;
                }
            }
            else {
                if ( orient( dest_positionIndex ) == 1 ) {
                    carClockwise();
                }
                else {
                    carCounterClockwise();
                }
                updateBuf(env.presentWorkingPtr);
            }
        }
    }
    else //当前有任务
    {
        if ( car.position == dest_positionIndex ) //当前请求已完成，判定后续节点请求可否同时完成
        {
            state = NO_TASK; //下一次clock指令调用本模块时再次进入working状态
            //FCFS_finishRequest( presentWorkingPtr->where, presentWorkingPtr->stationNumber );
            while ( env.presentWorkingPtr->next ) {
                if ( env.presentWorkingPtr->next->stationNumber == env.presentWorkingPtr->stationNumber ) {
                    env.presentWorkingPtr = env.presentWorkingPtr->next;
                    //FCFS_finishRequest( presentWorkingPtr->where, presentWorkingPtr->stationNumber );
                }
                else {
                    break;
                }
                //将指针定位到最后一个与当前请求相同的节点，以上请求视为全部同时完成
            }
            if (env.presentWorkingPtr->next){//如果后续有其他任务节点，传下一节点遍历至链表尾部更新数组
                updateBuf(env.presentWorkingPtr->next);
            }
            else{
                updateBuf(NULL);
            }
        }
        else {
            if ( orient( dest_positionIndex ) == 1 ) {
                carClockwise();
            }
            else {
                carCounterClockwise();
            }
            updateBuf(env.presentWorkingPtr);
        }
    }
}

void modeSCAN( void )
{
    // 状态变量，初始化为STOP
    static enum { STOP, CLOCKWISE_STOP, COUNTERCLOCKWISE_STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    //printf("=========================\n");
    //printf("s_dest_stationNumber:%d\n",s_dest_stationNumber);
    //printf("=========================\n");
    if ( state == STOP ){
        s_dest_stationNumber = SSTFfindNearestStationNumber();
        if( s_dest_stationNumber==-1){
            state = STOP;
        }
        else if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 无方向停车状态本站请求直接完成
            finishRequest(s_dest_stationNumber, 0, FALSE);
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
            if (nearsestDistence == 0) {
                finishRequest(getStationNumber(car.position), 0, TRUE);
            }
            nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 1 )), 1);
            if (nearsestDistence*2>env.TOTAL_STATION*env.DISTANCE){
                state=COUNTERCLOCKWISE;
                carCounterClockwise();
                s_dest_stationNumber=SCANfindNearestStationNumber(2);
            }
            else{
                state=CLOCKWISE;
                carClockwise();
                s_dest_stationNumber=SCANfindNearestStationNumber(1);
            }
        }
        else if ( state == COUNTERCLOCKWISE_STOP ) {//停车前行驶方向为逆时针
            int nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 2 )), 2);
            if (nearsestDistence == 0) {
                finishRequest(getStationNumber(car.position), 0, TRUE);
            }
            nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 2 )), 2);
            if (nearsestDistence*2>env.TOTAL_STATION*env.DISTANCE){
                state=CLOCKWISE;
                carClockwise();
                s_dest_stationNumber=SCANfindNearestStationNumber(1);
            }
            else{
                state=COUNTERCLOCKWISE;
                carCounterClockwise();
                s_dest_stationNumber=SCANfindNearestStationNumber(2);
            }
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = CLOCKWISE_STOP;
            finishRequest( s_dest_stationNumber ,0, TRUE); // 完成请求
            s_dest_stationNumber = SSTFfindNearestStationNumber();
            if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 原地请求，立即完成，不改变state
                finishRequest(getStationNumber(car.position), 0, TRUE);
            }
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = CLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) ,0, TRUE);
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( s_dest_stationNumber ,0, TRUE); // 完成请求
            s_dest_stationNumber = SSTFfindNearestStationNumber();
            if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 原地请求，立即完成，不改变state
                finishRequest(getStationNumber(car.position), 0, TRUE);
            }
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) ,0, TRUE);
        }
        else {
            carCounterClockwise();
        }
    }
    // 重置数组第二行
    for (int i = 0; i < 20; i++) {
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
            else if (minDistance == stationDistance(i + 1)) {
                if (orient(getPositionIndex(i + 1)) == 1) {
                    minDistance = stationDistance( i + 1 );
                    res = i + 1;
                }
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
            else if (minDistance == stationDistance(i + 1)) {
                if (orient(getPositionIndex(i + 1)) == 1) {
                    minDistance = stationDistance( i + 1 );
                    res = i + 1;
                }
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
            else if (minDistance == stationDistance(i + 1)) {
                if (orient(getPositionIndex(i + 1)) == 1) {
                    minDistance = stationDistance( i + 1 );
                    res = i + 1;
                }
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

void finishRequest( int stationNumber , int direction, int ifDelay)
{
    // **添加ifDelay参数用于判断在完成请求是是否考虑提前一秒。
    // 实现方式：如果ifDelay为TRUE，则本函数的行为和改动前一模一样（!ifDelay为FALSE，或一个FALSE相当于本身）。
    // 如果ifDelay为FALSE，即不考虑提前一秒原则，无论数组第二行是1还是0，请求都会完成。
    if ( car.target[ 0 ][ stationNumber - 1 ] == 1 && (car.target[1][stationNumber - 1] == 0 || !ifDelay)) {
        car.target[ 0 ][ stationNumber - 1 ] = 0;
    }
    if (direction == 0 || direction == 1 ) {
        if ( station.clockwise[ 0 ][ stationNumber - 1 ] == 1 && (station.clockwise[1][stationNumber - 1] == 0 || !ifDelay)) {
            station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
            //printf("clockwise完成请求: %d\n",stationNumber);
        }
    }
    if (direction == 0 || direction == 2 ) {
        if ( station.counterclockwise[ 0 ][ stationNumber - 1 ] == 1 && (station.counterclockwise[1][stationNumber - 1] == 0 || !ifDelay)) {
            station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
            //printf("counterclockwise完成请求: %d\n",stationNumber);
        }
    }
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
    if (clockwiseDistence == 0) {
        counterclockwiseDistence = 0;
    }
    else {
        counterclockwiseDistence = env.TOTAL_STATION * env.DISTANCE - clockwiseDistence;
    }
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

void updateBuf(NODE* presentPtr )
{
    for ( int i = 0; i < env.TOTAL_STATION; i++ ) {
        car.target[ 0 ][ i ] = 0;
        station.clockwise[ 0 ][ i ] = 0;
        station.counterclockwise[ 0 ][ i ] = 0;
    }
    car.target[ 0 ][ env.TOTAL_STATION] = -1;
    station.clockwise[ 0 ][ env.TOTAL_STATION] = -1;
    station.counterclockwise[ 0 ][ env.TOTAL_STATION] = -1;

    NODE* Nptr = presentPtr;
    while ( Nptr ) {
        if ( Nptr->where == 1 ) {
            car.target[ 0 ][ Nptr->stationNumber - 1 ] = 1;
        }
        else if ( Nptr->where == 2 ) {
            station.clockwise[ 0 ][ Nptr->stationNumber - 1 ] = 1;
        }
        else if ( Nptr->where == 3 ) {
            station.counterclockwise[ 0 ][ Nptr->stationNumber - 1 ] = 1;
        }
        Nptr = Nptr->next;
    }
}

//------------------------}}}内部函数实现
