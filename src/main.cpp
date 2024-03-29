#include "main.h"
#include "core.h"
#include "input.h"
#include "mainwindow.h"
#include "strategy.h"
#include <cstdio>
#include <qnamespace.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <QApplication>
#include <QThread>

extern ENVIRONMENT env;
extern CAR         car;
extern STATION     station;
extern int         TIME;

GLOB        global;
CoreThread* corethread;

void initMain(void)
{
    // init environment
    FILE* fPtr = NULL;
    if ((fPtr = fopen("dict.dic", "r")) == NULL) {
        printf("Can't open file \"dict.dic\"\n");
        exit(EXIT_FAILURE);
    }
    readfile(fPtr);
    fclose(fPtr);
    global.car_theta = 0;
    global.car_state = GLOB::STOP;
    global.glob_state_refresh = false;
    global.startGame = false;
    global.terminate = false;
}

int main(int argc, char** argv)
{
    initMain();
    corethread = new CoreThread();
    corethread->start();
    // TODO corethread需要terminate吗，应该放在哪？
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);
    MainWindow   w;
    w.show();
    return a.exec();
}
