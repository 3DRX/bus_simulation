#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern ENVIRONMENT env;

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
        } else {
            workingIndex++;
        }
        if ( state == NORMAL ) {
            // NORMAL模式下，靠第一个字符来判断是否是注释
            // 以及是三个变量中的哪个。
            // 如果是'\n'，换行。
            if ( workingIndex == 1 && ipt == '#' ) {
                state = COMMENT;
            } else if ( workingIndex == 1 && ipt == 'T' ) {
                state = TOTAL_STATION;
            } else if ( workingIndex == 1 && ipt == 'S' ) {
                state = STRATEGY;
            } else if ( workingIndex == 1 && ipt == 'D' ) {
                state = DISTANCE;
            } else if ( ipt == '\n' ) {
                workingIndex = 0;
            }
        } else if ( state == COMMENT ) {
            // COMMENT模式下，在检测到'\n'之后回到NORMAL模式。
            if ( ipt == '\n' ) {
                state = NORMAL;
                workingIndex = 0;
            }
        } else if ( state == TOTAL_STATION ) {
            // TOTAL_STATION模式，读取等号后的数据。
            if ( ipt == '=' ) {
                fscanf( fPtr, "%d", &env.TOTAL_STATION );
                state = NORMAL;
            }
        } else if ( state == STRATEGY ) {
            // STRATEGY模式，将等号后的字符串与FCFS,SSTF,SCAN
            // 对比（输入字符串一定是三者之一），
            // 相符的结果存在STRATEGY中。
            if ( ipt == '=' ) {
                char inputBuf[ 10 ];
                fscanf( fPtr, "%s", inputBuf );
                if ( strcmp( inputBuf, "FCFS" ) == 0 ) {
                    env.STRATEGY = FCFS;
                } else if ( strcmp( inputBuf, "SSTF" ) == 0 ) {
                    env.STRATEGY = SSTF;
                } else if ( strcmp( inputBuf, "SCAN" ) == 0 ) {
                    env.STRATEGY = SCAN;
                }
                state = NORMAL;
            }
        } else if ( state == DISTANCE ) {
            // DISTANCE模式，读取等号后的数据。
            if ( ipt == '=' ) {
                fscanf( fPtr, "%d", &env.DISTANCE );
                state = NORMAL;
            }
        }
    }
    // end read file
}

