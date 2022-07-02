#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "core.h"
#include "main.h"

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <qlineedit.h>
#include <qnamespace.h>
#include <qsize.h>
#include <sys/time.h>


#define PI 3.1415926535897932384626
#define FPS 15

extern ENVIRONMENT env;
extern GLOB global;
extern CAR car;
extern STATION station;
extern int TIME;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Bus-simulation");
    // 设置窗口、画布
    resize(1024, 768);
    pix = QPixmap(1024, 700);
    pix.fill(Qt::white);
    setMinimumSize(1024, 768);
    setMaximumSize(1024, 768);
    // 创建按钮
    button_start = new QPushButton(this);
    button_start->setText(tr("start"));
    button_start->move(0, 701);
    connect(button_start, SIGNAL(clicked()), this, SLOT(startPressed()));
    button_stop = new QPushButton(this);
    button_stop->setText(tr("stop"));
    button_stop->move(0, 740);
    connect(button_stop, SIGNAL(clicked()), this, SLOT(stopPressed()));
    button_stop->setDisabled(true);
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
    counterclockwise_string = (char*)malloc(sizeof(char) * (env.TOTAL_STATION + 1));
    clockwise_string = (char*)malloc(sizeof(char) * (env.TOTAL_STATION + 1));
    target_string = (char*)malloc(sizeof(char) * (env.TOTAL_STATION + 1));
    counterclockwise_string[env.TOTAL_STATION] = '\0';
    clockwise_string[env.TOTAL_STATION] = '\0';
    target_string[env.TOTAL_STATION] = '\0';
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::restartPressed()
{
    // global.ifWait = false;
    // TODO: 删除ifWait
    button_start->setText(tr("start"));
    connect(button_start, SIGNAL(clicked()), this, SLOT(startPressed()));
    button_stop->setDisabled(true);
    button_start->setDisabled(false);
    global.ifWait = true;
    global.car_theta = 0;
    global.car_state = GLOB::STOP;
    global.glob_state_refresh = false;
    global.startGame = false;
    initGame();
}

void MainWindow::startPressed()
{
    global.startGame = true;
    global.glob_state_refresh = true;
    button_start->setDisabled(true);
    button_stop->setDisabled(false);
}

void MainWindow::stopPressed()
{
    global.startGame = false;
    button_start->setDisabled(false);
    button_stop->setDisabled(true);
    button_start->setText(tr("restart"));
    connect(button_start, SIGNAL(clicked()), this, SLOT(restartPressed()));
}

int FCFS_checklist_gui(int where, int stationNumber)
{
    //用于检查新请求是否已有相同的未完成请求，若有则抛弃新请求
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

void MainWindow::inputPressed()
{
    QString Tinput = editT->text();
    QString Cinput = editC->text();
    QString Uinput = editU->text();
    editT->clear();
    editC->clear();
    editU->clear();
    if (env.STRATEGY == ENVIRONMENT::FCFS) {
        if (Tinput.toInt() <= env.TOTAL_STATION && Tinput.toInt() >= 1) {
            if (FCFS_checklist_gui(1, Tinput.toInt()) != 1) {
                NODE* prePtr = (NODE*)malloc(sizeof(NODE));
                env.presentPtr->next = prePtr;
                prePtr->prev = env.presentPtr;
                prePtr->next = NULL;
                env.presentPtr = env.presentPtr->next;
                env.presentPtr->where = 1;
                env.presentPtr->stationNumber = Tinput.toInt();
            }
        }
        else {
        }
        if (Cinput.toInt() <= env.TOTAL_STATION && Cinput.toInt() >= 1) {
            if (FCFS_checklist_gui(2, Tinput.toInt()) != 1) {
                NODE* prePtr = (NODE*)malloc(sizeof(NODE));
                env.presentPtr->next = prePtr;
                prePtr->prev = env.presentPtr;
                prePtr->next = NULL;
                env.presentPtr = env.presentPtr->next;
                env.presentPtr->where = 2;
                env.presentPtr->stationNumber = Cinput.toInt();
            }
        }
        else {
        }
        if (Uinput.toInt() <= env.TOTAL_STATION && Uinput.toInt() >= 1) {
            if (FCFS_checklist_gui(3, Tinput.toInt()) != 1) {
                NODE* prePtr = (NODE*)malloc(sizeof(NODE));
                env.presentPtr->next = prePtr;
                prePtr->prev = env.presentPtr;
                prePtr->next = NULL;
                env.presentPtr = env.presentPtr->next;
                env.presentPtr->where = 3;
                env.presentPtr->stationNumber = Uinput.toInt();
            }
        }
        else {
        }
    }
    else { // 非 FCFS 模式
        if (Tinput.toInt() <= env.TOTAL_STATION && Tinput.toInt() >= 1) {
            if (global.startGame == true) {
                if (car.target[0][Tinput.toInt() - 1] != 1) {
                    car.target[0][Tinput.toInt() - 1] = 1;
                    car.target[1][Tinput.toInt() - 1] = 1;
                }
            }
            else {
                car.target[0][Tinput.toInt() - 1] = 1;
                car.target[1][Tinput.toInt() - 1] = 1;
            }
        }
        else {
        }
        if (Cinput.toInt() <= env.TOTAL_STATION && Cinput.toInt() >= 1) {
            if (global.startGame == true) {
                if (station.clockwise[0][Cinput.toInt() - 1] != 1) {
                    station.clockwise[0][Cinput.toInt() - 1] = 1;
                    station.clockwise[1][Cinput.toInt() - 1] = 1;
                }
            }
            else {
                station.clockwise[0][Cinput.toInt() - 1] = 1;
                station.clockwise[1][Cinput.toInt() - 1] = 1;
            }
        }
        else {
        }
        if (Uinput.toInt() <= env.TOTAL_STATION && Uinput.toInt() >= 1) {
            if (global.startGame == true) {
                if (station.counterclockwise[0][Uinput.toInt() - 1] != 1) {
                    station.counterclockwise[0][Uinput.toInt() - 1] = 1;
                    station.counterclockwise[1][Uinput.toInt() - 1] = 1;
                }
            }
            else {
                station.counterclockwise[0][Uinput.toInt() - 1] = 1;
                station.counterclockwise[1][Uinput.toInt() - 1] = 1;
            }
        }
        else {
        }
    }
}

void MainWindow::paintEvent(QPaintEvent*)
{
    // 主绘图函数
    paintBackground();
    paintStations();
    if (global.startGame == true) {
        moveBus();
    }
    paintBus();
    paintoutput();
    paintlight();
    paintInput();
    paintstrategy();
    QPainter painter(this);
    painter.drawPixmap(0, 0, pix);
    pix.fill(Qt::white);
    update(); // 这个函数能令paintEvent不停的循环
}

void MainWindow::paintBackground(void)
{
    // the big circle
    // center: (390, 350)    radious: 300
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 6));
    QRectF rectangle(390 - 300, 350 - 300, 600, 600);
    p.drawEllipse(rectangle);
}

void MainWindow::paintStations(void)
{
    double theta = (double)360 / (env.TOTAL_STATION * env.DISTANCE);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(390, 350);
    for (int i = 0; i < (env.TOTAL_STATION * env.DISTANCE); i++) {
        double angle = theta * i;
        if ((i % env.DISTANCE) != 0) {
            // draw small circle
            QRectF rectangle(290, -10, 20, 20);
            p.setPen(Qt::black);
            p.setBrush(Qt::black);
            p.drawEllipse(rectangle);
        }
        else {
            char a[3];
            a[1] = '\0';
            a[2] = '\0';
            if (i == 0) {
                a[0] = '1';
            }
            else { // 咱就当他最多是10站吧
                if (i / env.DISTANCE == 9) {
                    a[0] = '1';
                    a[1] = '0';
                }
                else {
                    a[0] = '1' + i / env.DISTANCE;
                }
            }
            // draw big circle
            QRectF rectangle(280, -20, 40, 40);
            p.setPen(Qt::black);
            p.setBrush(Qt::black);
            p.drawEllipse(rectangle);
            p.save();
            // draw text
            // 转回原来的角度，使数字是正的
            p.rotate((double)(360 - (i * ((double)360 / (env.TOTAL_STATION * env.DISTANCE)))));
            QFont font("arial", 12, QFont::Bold, false);
            p.setFont(font);
            p.setPen(Qt::white);
            // 手动极坐标，+5-5是为了让字（而不是字的左下角）在圆中央
            p.drawText((300 * std::cos(angle * PI / 180)) - 10,
                (300 * std::sin(angle * PI / 180)) + 7, tr(a));
            p.restore();
        }
        p.rotate((double)360 / (env.TOTAL_STATION * env.DISTANCE));
    }
}

void MainWindow::paintBus(void)
{
    // 车两种方向的车头朝向变化是读取两个图片（懒得写代码去变换）
    // 车停车后会保持停车之前的车头朝向
    static auto last_state = GLOB::COUNTERCLOCKWISE;
    QPainter p(&pix);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.translate(390, 350);
    p.rotate(global.car_theta);
    QString imgname;
    if (global.car_state == GLOB::CLOCKWISE) {
        imgname = QString("./resources/bus_icon2.png");
        last_state = GLOB::CLOCKWISE;
    }
    else if (global.car_state == GLOB::COUNTERCLOCKWISE) {
        imgname = QString("./resources/bus_icon.png");
        last_state = GLOB::COUNTERCLOCKWISE;
    }
    else {
        if (last_state == GLOB::CLOCKWISE) {
            imgname = QString("./resources/bus_icon2.png");
        }
        else {
            imgname = QString("./resources/bus_icon.png");
        }
    }
    QPixmap pixmap(imgname);
    p.drawPixmap(210, -30, 60, 60, pixmap);
}

void MainWindow::moveBus(void)
{
    static int last_time = 10000;
    struct timeval tp;
    int ms;
    gettimeofday(&tp, NULL);
    ms = tp.tv_usec / 1000;
    // 我也不知道这个if是怎么使代码正确运行起来的
    // 反正经过一通魔幻操作，它就是跑起来了
    if (last_time == 10000 || ms - last_time >= 1000 / FPS || ms - last_time < 0) {
        if (last_time == 10000) {
            last_time = ms;
        }
        else if (last_time >= 1000 - 1000 / FPS) {
            last_time = 0;
        }
        else {
            last_time += 1000 / FPS;
        }
        if (global.car_state == GLOB::CLOCKWISE) {
            if (global.car_theta + 1 == 360) {
                global.car_theta = 0;
            }
            else {
                global.car_theta++;
            }
        }
        else if (global.car_state == GLOB::COUNTERCLOCKWISE) {
            if (global.car_theta - 1 == -1) {
                global.car_theta = 359;
            }
            else {
                global.car_theta--;
            }
        }
        else {
            last_time = 10000;
        }
    }
    // update bus position
    int theta = 360 / (env.TOTAL_STATION * env.DISTANCE);
    for (int i = 0; i < env.TOTAL_STATION * env.DISTANCE; i++) {
        if (global.car_theta == theta * i) {
            car.position = i;
            global.glob_state_refresh = true;
        }
    }
}

void MainWindow::paintoutput(void)
{
    //画出右上角长方形
    QPainter p(&pix);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.setPen(QPen(Qt::black, 2));
    QFont font("arial", 14, QFont::Bold, false);
    p.setFont(font);
    QRectF rectangle(700, 25, 300, 150);
    p.drawRect(rectangle);

    // fcfs链表输出为字符数
    if (env.STRATEGY == ENVIRONMENT::FCFS) {
        for (int i = 0; i < env.TOTAL_STATION; i++) {
            counterclockwise_string[i] = '0';
            clockwise_string[i] = '0';
            target_string[i] = '0';
        }
        NODE* Nptr = env.presentWorkingPtr;
        while (Nptr) {
            printf("%d\n", Nptr->where);
            if (Nptr->where == 1) {
                target_string[Nptr->stationNumber - 1] = '1';
            }
            else if (Nptr->where == 2) {
                clockwise_string[Nptr->stationNumber - 1] = '1';
            }
            else if (Nptr->where == 3) {
                counterclockwise_string[Nptr->stationNumber - 1] = '1';
            }
            Nptr = Nptr->next;
        }
        if (global.ifFCFSRequestFinished == 1) {
            if (env.presentWorkingPtr->where == 1) {
                target_string[env.presentWorkingPtr->stationNumber - 1] = '0';
            }
            else if (env.presentWorkingPtr->where == 2) {
                clockwise_string[env.presentWorkingPtr->stationNumber - 1] = '0';
            }
            else if (env.presentWorkingPtr->where == 3) {
                counterclockwise_string[env.presentWorkingPtr->stationNumber - 1] = '0';
            }
        }
    }
    else {
        for (int i = 0; i < env.TOTAL_STATION; i++) {
            counterclockwise_string[i] = '0' + station.counterclockwise[0][i];
            clockwise_string[i] = '0' + station.clockwise[0][i];
            target_string[i] = '0' + car.target[0][i];
        }
    }

    // counterclockwise的输出
    QString s1 = counterclockwise_string;
    s1.prepend("counterclockwise:");
    p.translate(700, 25);
    p.drawText(20, 140, s1);
    QRectF rectangle11(5, 130, 10, 10); //右上角红色图例
    p.setPen(Qt::black);
    p.setBrush(Qt::red);
    p.drawEllipse(rectangle11);

    // clockwise的输出
    QString s2 = clockwise_string;
    QString s22 = QString("clockwise:") + s2;
    p.drawText(20, 110, s22);
    QRectF rectangle22(5, 100, 10, 10); //右上角黄色图例
    p.setPen(Qt::black);
    p.setBrush(Qt::yellow);
    p.drawEllipse(rectangle22);

    // target的输出
    QString s3 = target_string;
    s3.prepend("target:");
    p.drawText(20, 80, s3);
    QRectF rectangle33(5, 70, 10, 10); //右上角蓝色图例
    p.setPen(Qt::black);
    p.setBrush(Qt::blue);
    p.drawEllipse(rectangle33);

    // position的输出
    QString s4 = QString();
    s4.setNum(car.position, 10);
    s4.prepend("position:");
    p.drawText(5, 50, s4);

    // TIME的输出
    QString s5 = QString();
    s5.setNum(TIME, 10);
    s5.prepend("TIME:");
    p.drawText(5, 20, s5);
}

void MainWindow::paintlight(void) //画出站点指示灯
{
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(390, 350);
    for (int i = 0; i < (env.TOTAL_STATION); i++) {
        // draw light
        QRectF rectangle1(330, -25, 10, 10);
        p.setPen(Qt::black);
        if (counterclockwise_string[i] == '0') {
            p.setBrush(Qt::gray);
        }
        else {
            p.setBrush(Qt::red);
        }
        p.drawEllipse(rectangle1);

        QRectF rectangle2(330, -5, 10, 10);
        if (clockwise_string[i] == '0') {
            p.setBrush(Qt::gray);
        }
        else {
            p.setBrush(Qt::yellow);
        }
        p.drawEllipse(rectangle2);

        QRectF rectangle3(330, 15, 10, 10);
        if (target_string[i] == '0') {
            p.setBrush(Qt::gray);
        }
        else {
            p.setBrush(Qt::blue);
        }
        p.drawEllipse(rectangle3);
        p.rotate((double)360 / (env.TOTAL_STATION));
    }
}

void MainWindow::paintInput(void)
{
    //画出右下角长方形
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 2));
    QFont font("arial", 25, QFont::Bold, false);
    p.setFont(font);
    QRectF rectangle(700, 430, 300, 250);
    p.drawRect(rectangle);
    p.translate(700, 430);
    //画T C U
    QRectF rectangleT(20, 20, 40, 40);
    p.drawEllipse(rectangleT);
    p.drawText(30, 50, QString("T"));
    QRectF rectangleC(20, 20 + 50, 40, 40);
    p.drawEllipse(rectangleC);
    p.drawText(30, 50 + 50, QString("C"));
    QRectF rectangleU(20, 20 + 100, 40, 40);
    p.drawEllipse(rectangleU);
    p.drawText(30, 50 + 100, QString("U"));
    // 画文本框
    editT->show();
    editC->show();
    editU->show();
}

void MainWindow::paintstrategy(void)
{
    //画出左上角长方形
    QPainter p(&pix);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    p.setPen(QPen(Qt::black, 2));
    QFont font("arial", 18, QFont::Bold, false);
    p.setFont(font);
    QRectF rectangle(50, 25, 100, 40);
    p.drawRect(rectangle);

    //写入策略
    QString s;
    if(env.STRATEGY==ENVIRONMENT::FCFS)
    {
        s ="FCFS";
    }
    else if(env.STRATEGY==ENVIRONMENT::SCAN)
    {
        s ="SCAN";
    }
    else if(env.STRATEGY==ENVIRONMENT::SSTF)
    {
        s ="SSTF";
    }
    p.translate(50, 25);
    p.drawText(20, 30, s);

}

