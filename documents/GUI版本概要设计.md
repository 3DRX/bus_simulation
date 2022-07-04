# GUI版本概要设计

## 数据结构设计

### 全局变量

```cpp
typedef struct {
    int TOTAL_STATION;
    int DISTANCE;
    enum { FCFS,
           SSTF,
           SCAN } STRATEGY;
    struct Node* headnode;
    struct Node* presentPtr;
    struct Node* presentWorkingPtr;
} ENVIRONMENT;

// 数组第二行定义：
// 为0则代表不是这一秒的新增请求
// 为1代表是这一秒的新增请求
typedef struct {
    int position;
    int target[2][21];
} CAR;

typedef struct {
    int clockwise[2][21];
    int counterclockwise[2][21];
} STATION;

typedef struct Node {
    struct Node* prev;          // always NULL in headnode
    int          where;         // -1 not FCFS; 1 car.target; 2 clockwise; 3 counterclockwise
    int          stationNumber; // -1 not FCFS
    struct Node* next;
} NODE;

// 用于线程之间共享信息
typedef struct {
    //用于指示fcfs模式下当前指向的节点任务是否完成
    int ifFCFSRequestFinished;

    // 车的位置（角度theta）
    int car_theta;
    enum { CLOCKWISE,
           COUNTERCLOCKWISE,
           STOP } car_state;

    // 由车到每一个小站点触发，
    // strategy模块依靠这个判断是否更新策略
    bool glob_state_refresh;

    // 控制游戏的暂停、继续
    bool startGame;

    // 控制游戏是否结束
    // （是否释放内存，结束进程）
    bool terminate;
} GLOB;
```


```cpp
ENVIRONMENT env;
CAR         car;
STATION     station;
int         TIME = 0;
GLOB        global;
CoreThread* corethread;
```


## 模块划分

### - 输入

1. 从文件中读取配置文件
   
   ```cpp
   void readfile(FILE* fPtr);
   ```

2. 回收用于存储FCFS指令的链表内存
   
   ```cpp
   void FCFS_freeList(NODE* headPtr);
   ```

3. GUI上的按钮与文本输入框
   
   按钮的回调函数：
   
   ```cpp
   // 按restart按钮触发的回调函数
   void continuePressed();
   
   // 按start按钮触发的回调函数
   void startPressed();
   
   // 按stop按钮触发的回调函数
   void stopPressed();
   
   // 按input按钮触发的回调函数
   void inputPressed();
   
   // 按end按钮触发的回调函数
   void endPressed();
   ```
   
   > 注：此处回调函数与按钮并不一一对应，同一个按钮在不同的状态下会连接至不同的函数。详见按钮回调函数的实现以及MainWindow类的构造函数实现。
   
    
   
   MainWindow构造函数中输入相关对象的创建：
   
   ```cpp
   // 创建按钮
   button_start = new QPushButton(this);
   button_start->setText(tr("start"));
   button_start->move(0, 701);
   connect(button_start, SIGNAL(clicked()), this, SLOT(startPressed()));
   button_end = new QPushButton(this);
   button_end->setText(tr("end"));
   button_end->move(0, 735);
   connect(button_end, SIGNAL(clicked()), this, SLOT(endPressed()));
   
   // 创建文本框以及确认按钮
   editT = new QLineEdit(this);
   editC = new QLineEdit(this);
   editU = new QLineEdit(this);
   editT->setGeometry(700 + 90, 440 + 20, 100, 20);
   editC->setGeometry(700 + 90, 440 + 20 + 50, 100, 20);
   editU->setGeometry(700 + 90, 440 + 20 + 100, 100, 20);
   button_input = new QPushButton(this);
   button_input->setText(tr("input"));
   button_input->move(900, 701);
   connect(button_input, SIGNAL(clicked()), this, SLOT(inputPressed()));
   ```

### - 策略

1. 计时器的创建与使用

```cpp
// 创建计时器（用于控制车速和显示游戏时间）
busTimer = new QTimer(this);
busTimer->QTimer::setTimerType(Qt::PreciseTimer);
connect(busTimer, SIGNAL(timeout()), SLOT(busTimeout()));
busTimer->setInterval(1);
busTimer->start(20);
```

```cpp
// 计时器回调函数
void busTimeout();
```

2. 三种策略函数

```cpp
void modeFCFS();
void modeSSTF();
void modeSCAN();
```

内部函数：

> **stationNumber**: 站点的序号，从 1 开始  
> **positionIndex**: 位置的索引，从 0 开始  
> 一个 station 中间会有若干个 position ，本问题涉及两个序列。在此对命名进行规范。

1. 


```c
/**计算当前位置与目标位置之间的距离。
 * stationNumber：目标站号（从1开始）
 */
int stationDistance( int stationNumber );
```

2. 


```c
/**寻找当前所有请求中最短的那个，
 * 返回到达用时最短的站台编号（从1开始）。
 * 如果无请求，就返回-1
 */
int findNearestStationNumber( void );
```

3. 


```c
/**通过positionIndex计算对应的stationNumber，
 * 如果position在站点上，返回站台编号，
 * 如果所在position不是站点，返回-1。
 */
int getStationNumber( int positionIndex );
```

4. 


```c
/**通过stationNumber计算对应的positionIndex，
 */
int getPositionIndex( int stationNumber );
```

5.  


```c
/**完成输入stationNumber的请求
 * （把数组中的1改成0）
 */
void finishRequest(int stationNumber);
```

6. 


```c
/**车辆顺时针移动一个单位长度
 */
void carClockwise(void);

/**车辆逆时针移动一个单位长度
 */
void carCounterClockwise(void);

/**车辆原地不动
 */
void carStop(void);
```

7.  


```c
/**检测car当前的位置有没有请求
 * direction: 1 顺时针，2 逆时针
 * 如果有，返回TRUE
 * 如果没有，返回FALSE
 * 如果当前位置不是车站，返回-1
 */
short haveRequest( short direction );
```

8. 


```c
/**用于FCFS策略中完成请求时从全局变量environment和car中去除对应请求
 */
void FCFS_finishRequest( int where, int stationNumber );
```

9. 


```c
/*用于计算车前往当前目标站点采用的方向,顺时针返回1，逆时针返回2.
 */
int orient( int stationPosition );
```

10. 


```c
//返回某一方向的距离
int SCAN_stationDistance( int stationPosition, int state );
```

11.  


```c
void FCFS_haveOnStationRequest( NODE* presentWorkingPtr );
```

12.  


```c
void updateBuf( NODE* presentPtr );
```

13.  


```c
//用于检测是否有非当前位置请求，有则返回1，无则返回0
int AreThereAnyRequest( void );
```


### - 输出（显示）

外部函数：


```cpp
void paintEvent(QPaintEvent*);
```


内部函数：


```cpp
/**绘制大圆轨道和小圆点车站
 */
void paintBackground(void);
void paintStations(void);

/**绘制公交车
 */
void paintBus(void);
void moveBus(void);

/**绘制输出区和指示灯
 */
void paintoutput(void);
void paintlight(void);
void paintstrategy(void);

/**绘制输入区
 */
void paintInput(void);
```
