#ifndef INPUT_H
#define INPUT_H

#include "main.h"
#include <stdio.h>

void readfile(FILE *fPtr);

/**从命令行中读取指令，
 * 函数运行一次只读取一行，
 * 读取到clock之后将TIME加一。
 */
void readOrder();

void FCFS_readOrder();

void FCFS_freeList(NODE *headPtr);

#endif
