#ifndef INPUT_H
#define INPUT_H

#include "main.h"
#include <stdio.h>

void readfile( FILE* fPtr );

order* readOrder();

digitalOrder* getMemory();

void createList();

#endif
