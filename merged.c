// #includes/headers 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef MAIN_H
#define MAIN_H

#define TRUE ( 1 )
#define FALSE ( 0 )
#define END ( -1 )
#define DBG (-2)

typedef struct {
    int TOTAL_STATION;
    int DISTANCE;
    enum { FCFS, SSTF, SCAN } STRATEGY;
    struct Node * headnode;
    struct Node * presentPtr;
    struct Node * presentWorkingPtr;
} ENVIRONMENT;

// 数组第二行定义：
// 为0则代表不是这一秒的新增请求
// 为1代表是这一秒的新增请求
typedef struct {
    int position;
    int target[ 2 ][ 21 ];
} CAR;

typedef struct {
    int clockwise[ 2 ][ 21 ];
    int counterclockwise[ 2 ][ 21 ];
} STATION;

typedef struct Node{
    struct Node *prev; // always NULL in headnode
    int where; // -1 not FCFS; 1 car.target; 2 clockwise; 3 counterclockwise
    int stationNumber; // -1 not FCFS
    struct Node *next;
} NODE;

#endif
// end main.h

#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>

void readfile( FILE* fPtr );

/**从命令行中读取指令，
 * 函数运行一次只读取一行，
 * 读取到clock之后将TIME加一。
 */
void readOrder();

void FCFS_readOrder();

void FCFS_freeList( NODE* headPtr );

#endif
// end input.h

#ifndef OUTPUT_H
#define OUTPUT_H


/**
 * 参数：ifOutPut
 * 为 TRUE 输出，
 * 为 FALSE 不输出，
 * 为 END 输出结束行。
 */
void outPut();

void printLines( void );

#endif
// end output.h

#ifndef STRATEGY_H
#define STRATEGY_H


void strategy( void );

#endif
// end strategy.h


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
        // 将数组的第二行全部置0
        car.target[ 1 ][ i ] = 0;
        station.clockwise[ 1 ][ i ] = 0;
        station.counterclockwise[ 1 ][ i ] = 0;
    }
    car.target[ 0 ][ env.TOTAL_STATION] = -1;
    station.clockwise[ 0 ][ env.TOTAL_STATION] = -1;
    station.counterclockwise[ 0 ][ env.TOTAL_STATION] = -1;
    // 上面三行中 -1 表示行结束（类似'\0'）
    // 如果是FCFS模式，初始化链表
    if ( env.STRATEGY == FCFS ) {
        env.headnode = malloc( sizeof( NODE ) );
        env.headnode->prev = NULL;
        env.headnode->next = NULL;
        env.headnode->stationNumber = -1;
        env.headnode->where = -1;
        env.presentPtr = env.headnode;
    }
    else {
        env.headnode = NULL;
    }
    // 初始化env.presentWorkingPtr
    env.presentWorkingPtr = NULL;
    // 输出TIME: 0
    printLines();
}

void mainLoop( void )
{
    if (env.STRATEGY == FCFS) {
        FCFS_readOrder();
    }
    else {
        readOrder();
    }
    strategy();
    outPut();
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

// #input.c

void readfile( FILE* fPtr ) // 从文件中读取环境初始配置
{
    int workingIndex = 0;
    // 自动机的状态
    enum { COMMENT, NORMAL, TOTAL_STATION, STRATEGY, DISTANCE } state;
    state = NORMAL;
    env.TOTAL_STATION = 5;
    env.STRATEGY = FCFS;
    env.DISTANCE = 2;
    char ipt;
    // start read file
    while ( 1 ) {
        if ( ( fscanf( fPtr, "%c", &ipt ) ) == EOF ) {
            break;
        }
        else {
            workingIndex++;
        }
        if ( state == NORMAL ) {
            // NORMAL模式下，靠第一个字符来判断是否是注释
            // 以及是三个变量中的哪个。
            // 如果是'\n'，换行。
            if ( workingIndex == 1 && ipt == '#' ) {
                state = COMMENT;
            }
            else if ( workingIndex == 1 && ipt == 'T' ) {
                state = TOTAL_STATION;
            }
            else if ( workingIndex == 1 && ipt == 'S' ) {
                state = STRATEGY;
            }
            else if ( workingIndex == 1 && ipt == 'D' ) {
                state = DISTANCE;
            }
            else if ( ipt == '\n' ) {
                workingIndex = 0;
            }
        }
        else if ( state == COMMENT ) {
            // COMMENT模式下，在检测到'\n'之后回到NORMAL模式。
            if ( ipt == '\n' ) {
                state = NORMAL;
                workingIndex = 0;
            }
        }
        else if ( state == TOTAL_STATION ) {
            // TOTAL_STATION模式，读取等号后的数据。
            if ( ipt == '=' ) {
                fscanf( fPtr, "%d", &env.TOTAL_STATION );
                state = NORMAL;
            }
        }
        else if ( state == STRATEGY ) {
            // STRATEGY模式，将等号后的字符串与FCFS,SSTF,SCAN
            // 对比（输入字符串一定是三者之一），
            // 相符的结果存在STRATEGY中。
            if ( ipt == '=' ) {
                char inputBuf[ 10 ];
                fscanf( fPtr, "%s", inputBuf );
                if ( strcmp( inputBuf, "FCFS" ) == 0 ) {
                    env.STRATEGY = FCFS;
                }
                else if ( strcmp( inputBuf, "SSTF" ) == 0 ) {
                    env.STRATEGY = SSTF;
                }
                else if ( strcmp( inputBuf, "SCAN" ) == 0 ) {
                    env.STRATEGY = SCAN;
                }
                state = NORMAL;
            }
        }
        else if ( state == DISTANCE ) {
            // DISTANCE模式，读取等号后的数据。
            if ( ipt == '=' ) {
                fscanf( fPtr, "%d", &env.DISTANCE );
                state = NORMAL;
            }
        }
    }
    // end read file
}

/**从命令行中读取指令，
 * 函数运行一次只读取一行，
 * 读取到clock之后将TIME加一。
 */
void readOrder()
{
    char inputBuff[20] = {'\0'};
    int inputNum = -1;
    scanf("%s",inputBuff);
    if (strcmp(inputBuff, "clock") == 0) {
        TIME++;
    }
    else if (strcmp(inputBuff, "clockwise") == 0) {
        scanf("%d", &inputNum);
        if (station.clockwise[0][inputNum-1] != 1) {
            station.clockwise[0][inputNum-1] = 1;
            station.clockwise[1][inputNum-1] = 1;
        }
    }
    else if (strcmp(inputBuff, "counterclockwise") == 0) {
        scanf("%d", &inputNum);
        if (station.counterclockwise[0][inputNum-1] != 1) {
            station.counterclockwise[0][inputNum-1] = 1;
            station.counterclockwise[1][inputNum-1] = 1;
        }
    }
    else if (strcmp(inputBuff, "target") == 0) {
        scanf("%d", &inputNum);
        if (car.target[0][inputNum-1] != 1) {
            car.target[0][inputNum-1] = 1;
            car.target[1][inputNum-1] = 1;
        }
    }
    else if (strcmp(inputBuff, "end") == 0) {
        TIME = -1;
    }
    else {
        printf( "     _     you fucked up !     _\n" );
        printf( "    |_|                       |_|\n" );
        printf( "    | |         /^^^\\         | |\n" );
        printf( "   _| |_      (| \"o\" |)      _| |_\n" );
        printf( " _| | | | _    (_---_)    _ | | | |_\n" );
        printf( "| | | | |' |    _| |_    | `| | | | |\n" );
        printf( "|          |   /     \\   |          |\n" );
        printf( " \\        /  / /(. .)\\ \\  \\        /\n" );
        printf( "   \\    /  / /  | . |  \\ \\  \\    /\n" );
        printf( "     \\  \\/ /    ||Y||    \\ \\/  /\n" );
        printf( "      \\__/      || ||      \\__/\n" );
        printf( "                () ()\n" );
        printf( "                || ||\n" );
        printf( "               ooO Ooo\n" );
    }
}

int FCFS_checklist(int where,int stationNumber)//用于检查新请求是否已有相同的未完成请求，若有则抛弃新请求
{
    NODE* presentPtr=env.presentWorkingPtr;
    while (presentPtr){
        if (where==presentPtr->where&&stationNumber==presentPtr->stationNumber){
            return 1;
        }
        else{
            presentPtr=presentPtr->next;
        }
    }
    return 0;
}

void FCFS_readOrder()
{
    char inputBuff[20] = {'\0'};
    int inputNum = -1;
    scanf("%s",inputBuff);
    if (strcmp(inputBuff, "clock") == 0) {
        TIME++;
    }
    else if (strcmp(inputBuff, "clockwise") == 0) {
        scanf("%d", &inputNum);
        if(FCFS_checklist(2,inputNum)!=1){
            NODE* prePtr = ( NODE* )malloc( sizeof( NODE ) );
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 2;
            env.presentPtr->stationNumber = inputNum;
        }
        else{
            //printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "counterclockwise") == 0) {
        scanf("%d", &inputNum);
        if(FCFS_checklist(3,inputNum)!=1){
            NODE* prePtr = ( NODE* )malloc( sizeof( NODE ) );
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 3;
            env.presentPtr->stationNumber = inputNum;
        }
        else{
            //printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "target") == 0) {
        scanf("%d", &inputNum);
        if(FCFS_checklist(1,inputNum)!=1){
            NODE* prePtr = ( NODE* )malloc( sizeof( NODE ) );
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 1;
            env.presentPtr->stationNumber = inputNum;
        }
        else{
            //printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "end") == 0) {
        TIME = -1;
    }
    else {
        printf( "     _     you fucked up !     _\n" );
        printf( "    |_|                       |_|\n" );
        printf( "    | |         /^^^\\         | |\n" );
        printf( "   _| |_      (| \"o\" |)      _| |_\n" );
        printf( " _| | | | _    (_---_)    _ | | | |_\n" );
        printf( "| | | | |' |    _| |_    | `| | | | |\n" );
        printf( "|          |   /     \\   |          |\n" );
        printf( " \\        /  / /(. .)\\ \\  \\        /\n" );
        printf( "   \\    /  / /  | . |  \\ \\  \\    /\n" );
        printf( "     \\  \\/ /    ||Y||    \\ \\/  /\n" );
        printf( "      \\__/      || ||      \\__/\n" );
        printf( "                () ()\n" );
        printf( "                || ||\n" );
        printf( "               ooO Ooo\n" );
    }
}

void FCFS_freeList( NODE* headPtr )
{
    NODE* ptr = headPtr;
    while ( headPtr ) {
        ptr = headPtr;
        headPtr = headPtr->next;
        free( ptr );
    }
}

// #output.c

void printLines( void )
{
    printf( "TIME:%d\n", TIME );
    printf( "BUS:\n" );
    printf( "position:%d\n", car.position );
    car.target[ 0 ][ 20 ] = -1; // 防越界
    printf( "target:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( car.target[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", car.target[ 0 ][ i ] );
    }
    printf( "\n" );
    printf( "STATION:\n" );
    station.clockwise[ 0 ][ 20 ] = -1;        // 防越界
    station.counterclockwise[ 0 ][ 20 ] = -1; // 防越界
    printf( "clockwise:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( station.clockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.clockwise[ 0 ][ i ] );
    }
    printf( "\n" );
    printf( "counterclockwise:" );
    for ( int i = 0; i < 20; i++ ) {
        if ( station.counterclockwise[ 0 ][ i ] == -1 ) {
            break;
        }
        printf( "%d", station.counterclockwise[ 0 ][ i ] );
    }
    printf( "\n" );
}

void outPut( int ifOutPut )
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
            printf( "end\n" );
            exit( EXIT_SUCCESS );
            if ( env.STRATEGY == FCFS ) {
                FCFS_freeList( env.headnode );
            }
        }
        if (TIME != 0) {
            printLines();
        }
    }
}

// #strategy.c

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
 */
void finishRequest( int stationNumber, int direction );


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

short targetCheck()
{
    int preStationNum=getStationNumber(car.position);
    if (car.target[1][preStationNum-1]==1)
    {
        return 1;//有新请求不置零
    }
    else 
    {
        return 0;
    }
} 
short clockwiseCheck()
{
    int preStationNum=getStationNumber(car.position);
    if (station.clockwise[1][preStationNum-1]==1)
    {
        return 1;//有新请求不置零
    }
    else 
    {
        return 0;
    }
} 

short counterclockwiseCheck()
{
    int preStationNum=getStationNumber(car.position);
    if (station.counterclockwise[1][preStationNum-1]==1)
    {
        return 1;//有新请求不置零
    }
    else 
    {
        return 0;
    }
} 


void modeSSTF( void )
{
    // 状态变量，初始化为STOP
    static enum { STOP, CLOCKWISE, COUNTERCLOCKWISE } state = STOP;
    static int s_dest_stationNumber = -1; // 目标站请求完成时被置-1
    static int if_update_2row = TRUE;
    //int checkTarget=0;
    //int checkClockwise=0;
    //int checkCounterclockwise=0;
    if ( state == STOP ) {
        if ( s_dest_stationNumber == -1 ) {
            // 如果上一个目标站请求完成，寻找找新的目标站
            s_dest_stationNumber = SSTFfindNearestStationNumber();
        }
        // 确定行驶方向
        if ( s_dest_stationNumber == -1 ) { // 如果当前没有请求，什么也不做
        }
        else if (s_dest_stationNumber == getStationNumber(car.position)) {
            // 原地请求，立即完成，不改变state
            finishRequest(s_dest_stationNumber, 0);
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber) ) == 1 ) {
            state = CLOCKWISE;
            carClockwise();
            // 检查挪车之后车的位置有没有新请求
            // 如果有，则停止刷新数组第二行一次
            //checkTarget=targetCheck();
            //checkClockwise=clockwiseCheck();
            //checkCounterclockwise=counterclockwiseCheck();
        }
        else if ( orient( getPositionIndex(s_dest_stationNumber )) == 2 ) {
            state = COUNTERCLOCKWISE;
            carCounterClockwise();
            // 检查挪车之后车的位置有没有新请求
            // 如果有，则停止刷新数组第二行一次
            //checkTarget=targetCheck();
            //checkClockwise=clockwiseCheck();
            //checkCounterclockwise=counterclockwiseCheck();
        }
    }
    else if ( state == CLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            /*printf("到站了\n");*/
            state = STOP;
            finishRequest( s_dest_stationNumber ,0); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( CLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            /*printf("途径站点\n");*/
            state = STOP;
            finishRequest( getStationNumber( car.position ) ,1);
        }
        else {
            carClockwise();
            // 检查挪车之后车的位置有没有新请求
            // 如果有，则停止刷新数组第二行一次
            //checkTarget=targetCheck();
            //checkClockwise=clockwiseCheck();
            //checkCounterclockwise=counterclockwiseCheck();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            //printf("到站了\n");
            state = STOP;
            finishRequest( s_dest_stationNumber ,0); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if ( haveRequest( COUNTERCLOCKWISE ) == TRUE ) { // 没到目标站但是途径站
            //printf("途径站点\n");
            state = STOP;
            finishRequest( getStationNumber( car.position ) ,2);
        }
        else {
            carCounterClockwise();
            // 检查挪车之后车的位置有没有新请求
            // 如果有，则停止刷新数组第二行一次
            //checkTarget=targetCheck();
            //checkClockwise=clockwiseCheck();
            //checkCounterclockwise=counterclockwiseCheck();
        }
    }
    else {
        printf("sth wrong\n");
    }
    //printf("===========================\n");
    //if (state == 0) {
        //printf("STATE: STOP\n");
    //}
    //else if (state == 1) {
        //printf("STATE: CLOCKWISE\n");
    //}
    //else if (state == 2) {
        //printf("STATE: COUNTERCLOCKWISE\n");
    //}
    //printf( "target:" );
    //for ( int i = 0; i < 20; i++ ) {
        //if ( car.target[ 1 ][ i ] == -1 ) {
            //break;
        //}
        //printf( "%d", car.target[ 1 ][ i ] );
    //}
    //printf( "\n" );
    //printf( "clockwise:" );
    //for ( int i = 0; i < 20; i++ ) {
        //if ( station.clockwise[ 1 ][ i ] == -1 ) {
            //break;
        //}
        //printf( "%d", station.clockwise[ 1 ][ i ] );
    //}
    //printf( "\n" );
    //printf( "counterclockwise:" );
    //for ( int i = 0; i < 20; i++ ) {
        //if ( station.counterclockwise[ 1 ][ i ] == -1 ) {
            //break;
        //}
        //printf( "%d", station.counterclockwise[ 1 ][ i ] );
    //}
    //printf( "\n" );
    //printf("===========================\n");
    // 重置数组第二行
    for (int i = 0; i < 20; i++) {
        car.target[1][i] = 0;
        station.clockwise[1][i] = 0;
        station.counterclockwise[1][i] = 0;
        }
    //if (checkTarget){
        //car.target[1][getStationNumber(car.position)-1]=1;
    //}
    //if (checkClockwise){
        //station.clockwise[1][getStationNumber(car.position)-1]=1;
    //}
    //if (checkCounterclockwise){
        //station.counterclockwise[1][getStationNumber(car.position)-1]=1;
    //}
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
                //FCFS_finishRequest( presentWorkingPtr->where,
                                        //presentWorkingPtr->stationNumber );
                while ( env.presentWorkingPtr->next ) {
                    if ( env.presentWorkingPtr->next->stationNumber
                         == env.presentWorkingPtr->stationNumber ) {
                        env.presentWorkingPtr = env.presentWorkingPtr->next;
                        //FCFS_finishRequest( presentWorkingPtr->where,
                                            //presentWorkingPtr->stationNumber );
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
                //FCFS_haveOnStationRequest(presentWorkingPtr);
                if ( orient( dest_positionIndex ) == 1 ) {
                    carClockwise();
                }
                else {
                    carCounterClockwise();
                }
                updateBuf(env.presentWorkingPtr);
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
    //SCAN策略相对于SSTF仅改变状态转移函数
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
        finishRequest(getStationNumber(car.position), 0);//停车状态直接完成本站请求
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
            //printf("====================\n");
            //printf("%d\n===================\n",nearsestDistence);
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
            //printf("====================\n");
            int nearsestDistence=SCAN_stationDistance(getPositionIndex(SCANfindNearestStationNumber( 2 )), 2);
            //printf("%d\n===================\n",nearsestDistence);
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
            finishRequest( s_dest_stationNumber, 0 ); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if (( haveRequest( CLOCKWISE )== TRUE) || (haveRequest( COUNTERCLOCKWISE ) == TRUE )) {
            // SCAN策略下无视请求类型，如果途径站点有请求则停车完成
            //printf("途径站点\n");
            //printf("haveRequest: %d, %d\n", haveRequest(CLOCKWISE), haveRequest(COUNTERCLOCKWISE));
            state = CLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) ,0);
        }
        else {
            carClockwise();
        }
    }
    else if ( state == COUNTERCLOCKWISE ) {
        if ( car.position == getPositionIndex( s_dest_stationNumber ) ) { // 说明到站了
            //printf("到站了\n");
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( s_dest_stationNumber ,0); // 完成请求
            s_dest_stationNumber = -1;             // 重置
        }
        else if (( haveRequest( CLOCKWISE )== TRUE) || (haveRequest( COUNTERCLOCKWISE ) == TRUE )) {
            // SCAN策略下无视请求类型，如果途径站点有请求则停车完成
            //printf("途径站点\n");
            //printf("==========================\n");
            //printf("haveRequest: %d, %d\n", haveRequest(CLOCKWISE), haveRequest(COUNTERCLOCKWISE));
            //printf("==========================\n");
            state = COUNTERCLOCKWISE_STOP;
            finishRequest( getStationNumber( car.position ) ,0);
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

void finishRequest( int stationNumber , int direction)
{
    // 完成请求前判断方向和提前一秒
    if ( car.target[ 0 ][ stationNumber - 1 ] == 1 && car.target[1][stationNumber - 1] == 0) {
        car.target[ 0 ][ stationNumber - 1 ] = 0;
    }
    if (direction == 0 || direction == 1 && station.clockwise[1][stationNumber - 1] == 0) {
        if ( station.clockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
            station.clockwise[ 0 ][ stationNumber - 1 ] = 0;
        }
    }
    if (direction == 0 || direction == 2 && station.counterclockwise[1][stationNumber - 1] == 0) {
        if ( station.counterclockwise[ 0 ][ stationNumber - 1 ] == 1 ) {
            station.counterclockwise[ 0 ][ stationNumber - 1 ] = 0;
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
