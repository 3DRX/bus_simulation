#include "core.h"
#include "input.h"
#include "main.h"
#include "mainwindow.h"
#include "output.h"
#include "strategy.h"
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <QApplication>
#include <QThread>

extern ENVIRONMENT env;
extern CAR car;
extern STATION station;
extern int TIME;

GLOB global;

void initMain(void) {
    // init environment
    FILE *fPtr = NULL;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) {
        printf("Can't open file \"dict.dic\"\n");
        exit(EXIT_FAILURE);
    }
    readfile(fPtr);
    fclose(fPtr);
    global.ifWait = true;
    global.fps = 30;
    global.car_theta = 0;
    global.car_state = GLOB::STOP;
}

int main(int argc, char **argv) {
    initMain();
    CoreThread *corethread;
    corethread = new CoreThread();
    corethread->start();
    // TODO corethread需要terminate吗，应该放在哪？
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
