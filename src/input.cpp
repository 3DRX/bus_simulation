#include "core.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern ENVIRONMENT env;
extern CAR car;
extern STATION station;
extern int TIME;

void readfile(FILE* fPtr) // 从文件中读取环境初始配置
{
    int workingIndex = 0;
    // 自动机的状态
    enum { COMMENT,
        NORMAL,
        TOTAL_STATION,
        STRATEGY,
        DISTANCE } state;
    state = NORMAL;
    env.TOTAL_STATION = 5;
    env.STRATEGY = ENVIRONMENT::FCFS;
    env.DISTANCE = 2;
    char ipt;
    // start read file
    while (1) {
        if ((fscanf(fPtr, "%c", &ipt)) == EOF) {
            break;
        }
        else {
            workingIndex++;
        }
        if (state == NORMAL) {
            // NORMAL模式下，靠第一个字符来判断是否是注释
            // 以及是三个变量中的哪个。
            // 如果是'\n'，换行。
            if (workingIndex == 1 && ipt == '#') {
                state = COMMENT;
            }
            else if (workingIndex == 1 && ipt == 'T') {
                state = TOTAL_STATION;
            }
            else if (workingIndex == 1 && ipt == 'S') {
                state = STRATEGY;
            }
            else if (workingIndex == 1 && ipt == 'D') {
                state = DISTANCE;
            }
            else if (ipt == '\n') {
                workingIndex = 0;
            }
        }
        else if (state == COMMENT) {
            // COMMENT模式下，在检测到'\n'之后回到NORMAL模式。
            if (ipt == '\n') {
                state = NORMAL;
                workingIndex = 0;
            }
        }
        else if (state == TOTAL_STATION) {
            // TOTAL_STATION模式，读取等号后的数据。
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.TOTAL_STATION);
                state = NORMAL;
            }
        }
        else if (state == STRATEGY) {
            // STRATEGY模式，将等号后的字符串与FCFS,SSTF,SCAN
            // 对比（输入字符串一定是三者之一），
            // 相符的结果存在STRATEGY中。
            if (ipt == '=') {
                char inputBuf[10];
                fscanf(fPtr, "%s", inputBuf);
                if (strcmp(inputBuf, "FCFS") == 0) {
                    env.STRATEGY = ENVIRONMENT::FCFS;
                }
                else if (strcmp(inputBuf, "SSTF") == 0) {
                    env.STRATEGY = ENVIRONMENT::SSTF;
                }
                else if (strcmp(inputBuf, "SCAN") == 0) {
                    env.STRATEGY = ENVIRONMENT::SCAN;
                }
                state = NORMAL;
            }
        }
        else if (state == DISTANCE) {
            // DISTANCE模式，读取等号后的数据。
            if (ipt == '=') {
                fscanf(fPtr, "%d", &env.DISTANCE);
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
    char inputBuff[20] = { '\0' };
    int inputNum = -1;
    fscanf(env.input, "%s", inputBuff);
    if (strcmp(inputBuff, "clock") == 0) {
        TIME++;
    }
    else if (strcmp(inputBuff, "clockwise") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (station.clockwise[0][inputNum - 1] != 1) {
            station.clockwise[0][inputNum - 1] = 1;
            station.clockwise[1][inputNum - 1] = 1;
        }
    }
    else if (strcmp(inputBuff, "counterclockwise") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (station.counterclockwise[0][inputNum - 1] != 1) {
            station.counterclockwise[0][inputNum - 1] = 1;
            station.counterclockwise[1][inputNum - 1] = 1;
        }
    }
    else if (strcmp(inputBuff, "target") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (car.target[0][inputNum - 1] != 1) {
            car.target[0][inputNum - 1] = 1;
            car.target[1][inputNum - 1] = 1;
        }
    }
    else if (strcmp(inputBuff, "end") == 0) {
        TIME = -1;
    }
    else {
        printf("     _     you fucked up !     _\n");
        printf("    |_|                       |_|\n");
        printf("    | |         /^^^\\         | |\n");
        printf("   _| |_      (| \"o\" |)      _| |_\n");
        printf(" _| | | | _    (_---_)    _ | | | |_\n");
        printf("| | | | |' |    _| |_    | `| | | | |\n");
        printf("|          |   /     \\   |          |\n");
        printf(" \\        /  / /(. .)\\ \\  \\        /\n");
        printf("   \\    /  / /  | . |  \\ \\  \\    /\n");
        printf("     \\  \\/ /    ||Y||    \\ \\/  /\n");
        printf("      \\__/      || ||      \\__/\n");
        printf("                () ()\n");
        printf("                || ||\n");
        printf("               ooO Ooo\n");
    }
}

int FCFS_checklist(
    int where,
    int stationNumber) //用于检查新请求是否已有相同的未完成请求，若有则抛弃新请求
{
    NODE* presentPtr = env.presentWorkingPtr;
    while (presentPtr) {
        if (where == presentPtr->where && stationNumber == presentPtr->stationNumber) {
            return 1;
        }
        else {
            presentPtr = presentPtr->next;
        }
    }
    return 0;
}

void FCFS_readOrder()
{
    char inputBuff[20] = { '\0' };
    int inputNum = -1;
    fscanf(env.input, "%s", inputBuff);
    if (strcmp(inputBuff, "clock") == 0) {
        TIME++;
    }
    else if (strcmp(inputBuff, "clockwise") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (FCFS_checklist(2, inputNum) != 1) {
            NODE* prePtr = (NODE*)malloc(sizeof(NODE));
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 2;
            env.presentPtr->stationNumber = inputNum;
        }
        else {
            // printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "counterclockwise") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (FCFS_checklist(3, inputNum) != 1) {
            NODE* prePtr = (NODE*)malloc(sizeof(NODE));
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 3;
            env.presentPtr->stationNumber = inputNum;
        }
        else {
            // printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "target") == 0) {
        fscanf(env.input, "%d", &inputNum);
        if (FCFS_checklist(1, inputNum) != 1) {
            NODE* prePtr = (NODE*)malloc(sizeof(NODE));
            env.presentPtr->next = prePtr;
            prePtr->prev = env.presentPtr;
            prePtr->next = NULL;
            env.presentPtr = env.presentPtr->next;
            env.presentPtr->where = 1;
            env.presentPtr->stationNumber = inputNum;
        }
        else {
            // printf("F_U\n");//用于debug
        }
    }
    else if (strcmp(inputBuff, "end") == 0) {
        TIME = -1;
    }
    else {
        printf("     _     you fucked up !     _\n");
        printf("    |_|                       |_|\n");
        printf("    | |         /^^^\\         | |\n");
        printf("   _| |_      (| \"o\" |)      _| |_\n");
        printf(" _| | | | _    (_---_)    _ | | | |_\n");
        printf("| | | | |' |    _| |_    | `| | | | |\n");
        printf("|          |   /     \\   |          |\n");
        printf(" \\        /  / /(. .)\\ \\  \\        /\n");
        printf("   \\    /  / /  | . |  \\ \\  \\    /\n");
        printf("     \\  \\/ /    ||Y||    \\ \\/  /\n");
        printf("      \\__/      || ||      \\__/\n");
        printf("                () ()\n");
        printf("                || ||\n");
        printf("               ooO Ooo\n");
    }
}

void FCFS_freeList(NODE* headPtr)
{
    NODE* ptr = headPtr;
    while (headPtr) {
        ptr = headPtr;
        headPtr = headPtr->next;
        free(ptr);
    }
}
