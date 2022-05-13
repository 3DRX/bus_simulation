# 概要设计

## 数据结构设计

### 常量定义

> 开发时按需添加

### 全局变量定义

```c
ENVIRONMENT * env;
CAR * car;
STATION * station;
```

### 数据结构定义

```C
typedef struct{
    int TOTAL_STATION;
    int DISTANCE;
    enum {FCFS, SSTF, SCAN} STRATEGY;
} ENVIRONMENT;
// 配置文件中的环境

typedef struct{
    int position;
    int target;
} CAR;
// 车对象

typedef struct{
    int clockwise;
    int counterclockwise;
} STATION;
// 站台对象

// 链表：digitalOrder
```

## 系统模块划分

main.c input.c output.c strategy.c control.c

1. main.c：主函数

2. input.c：读文件、读指令

3. output.c：输出TIME、BUS、STATION的状态（后续版本增加图形界面输出）

4. strategy.c：策略
   根据当前车内请求和站台请求，结合所采用的策略，判断车应采取的行为

5. control.c：根据策略改变车和STATION的状态

![1221652012089_.pic.jpg](概要设计_assets/62a6296dcd1d5ae39a9acf5b915fe2247040e103.jpg)

**input**

1. 读取配置文件
   `void readfile(FILE * fPtr);`
- [x] 康京旸

2. 读取输入的字符串
   `order * readOrder();`
- [ ] 盛泽桓

3. 为命令申请内存
   `digitalOrder * getMemory();`
- [ ] 盛泽桓

4. 将命令储存为链表
   `void createList();`
- [ ] 盛泽桓

**output**

1. 输出车状态、输出站台状态、输出时间
   `void output();`
- [x] 康京旸

**strategy**

1. `void FCFS(digitalOrder * ORDER);`

2. `void SSTF(digitalOrder * ORDER);`

3. `void SCAN(digitalOrder * ORDER);`

**control**

1. 控制移动
   `void car_control(int destination, int * buf);`

2. 改变station数据
   `void station_control(int * buf);`
