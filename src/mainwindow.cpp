#include "./ui_mainwindow.h"
#include "core.h"
#include "main.h"
#include "mainwindow.h"

#include <QDialog>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <cmath>
#include <cstdio>
#include <qnamespace.h>
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
    // 创建按钮
    button_next = new QPushButton(this);
    button_next->setText(tr("next"));
    button_next->move(920, 701);
    connect(button_next, SIGNAL(clicked()), this, SLOT(next()));
    button_previous = new QPushButton(this);
    button_previous->setText(tr("previous"));
    button_previous->move(0, 701);
    connect(button_previous, SIGNAL(clicked()), this, SLOT(previous()));
    button_clockwise = new QPushButton(this);
    button_clockwise->setText(tr("clockwise"));
    button_clockwise->move(920, 740);
    connect(button_clockwise, SIGNAL(clicked()), this,
        SLOT(clockwisePressed()));
    button_counterclockwise = new QPushButton(this);
    button_counterclockwise->setText(tr("counterclockwise"));
    button_counterclockwise->move(0, 740);
    connect(button_counterclockwise, SIGNAL(clicked()), this,
        SLOT(counterclockwisePressed()));
    button_stop = new QPushButton(this);
    button_stop->setText(tr("stop"));
    button_stop->move(460, 740);
    connect(button_stop, SIGNAL(clicked()), this, SLOT(stopPressed()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::next() { global.ifWait = false; }

void MainWindow::previous() { printf("button_previous pressed\n"); }

void MainWindow::clockwisePressed()
{
    if (global.car_state == GLOB::STOP) {
        global.car_state = GLOB::CLOCKWISE;
        button_clockwise->setDisabled(true);
        button_counterclockwise->setDisabled(true);
    }
}

void MainWindow::counterclockwisePressed()
{
    if (global.car_state == GLOB::STOP) {
        global.car_state = GLOB::COUNTERCLOCKWISE;
        button_clockwise->setDisabled(true);
        button_counterclockwise->setDisabled(true);
    }
}

void MainWindow::stopPressed()
{
    global.car_state = GLOB::STOP;
    button_counterclockwise->setDisabled(false);
    button_clockwise->setDisabled(false);
}

void MainWindow::paintEvent(QPaintEvent*)
{
    // 主绘图函数
    paintBackground();
    paintStations();
    moveBus();
    paintBus();
    paintoutput();
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
        } else {
            char a[3];
            a[1] = '\0';
            a[2] = '\0';
            if (i == 0) {
                a[0] = '1';
            } else { // 咱就当他最多是10站吧
                if (i / env.DISTANCE == 9) {
                    a[0] = '1';
                    a[1] = '0';
                } else {
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
    p.setRenderHint(QPainter::Antialiasing);
    p.translate(390, 350);
    p.rotate(global.car_theta);
    QString imgname;
    if (global.car_state == GLOB::CLOCKWISE) {
        imgname = QString("./resources/bus_icon2.png");
        last_state = GLOB::CLOCKWISE;
    } else if (global.car_state == GLOB::COUNTERCLOCKWISE) {
        imgname = QString("./resources/bus_icon.png");
        last_state = GLOB::COUNTERCLOCKWISE;
    } else {
        if (last_state == GLOB::CLOCKWISE) {
            imgname = QString("./resources/bus_icon2.png");
        } else {
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
        } else if (last_time >= 1000 - 1000 / FPS) {
            last_time = 0;
        } else {
            last_time += 1000 / FPS;
        }
        if (global.car_state == GLOB::CLOCKWISE) {
            if (global.car_theta + 1 == 360) {
                global.car_theta = 0;
            } else {
                global.car_theta++;
            }
        } else if (global.car_state == GLOB::COUNTERCLOCKWISE) {
            if (global.car_theta - 1 == -1) {
                global.car_theta = 359;
            } else {
                global.car_theta--;
            }
        } else {
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
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(QPen(Qt::black, 2));
    QFont font("arial", 10, QFont::Bold, false);
    p.setFont(font);
    QRectF rectangle(700, 25, 300, 150);
    p.drawRect(rectangle);

    char counterclockwise_string[env.TOTAL_STATION + 1];
    counterclockwise_string[env.TOTAL_STATION] = '\0';
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        counterclockwise_string[i] = '0' + station.counterclockwise[0][i];
    }
    QString s1 = counterclockwise_string;
    s1.prepend("counterclockwise:");
    p.translate(700, 25);
    p.drawText(5, 140, s1);

    char clockwise_string[env.TOTAL_STATION + 1];
    clockwise_string[env.TOTAL_STATION] = '\0';
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        clockwise_string[i] = '0' + station.clockwise[0][i];
    }
    QString s2 = clockwise_string;
    QString s22 = QString("clockwise:") + s2;
    p.drawText(5, 110, s22);

    char target_string[env.TOTAL_STATION + 1];
    target_string[env.TOTAL_STATION] = '\0';
    for (int i = 0; i < env.TOTAL_STATION; i++) {
        target_string[i] = '0' + car.target[0][i];
    }
    QString s3 = target_string;
    s3.prepend("target:");
    p.drawText(5, 80, s3);

    char position_string[3];
    position_string[2] = '\0';
    if (car.position <= 9) {
        position_string[1] = '\0';
        position_string[0] = '0' + car.position;
    } else {
        position_string[0] = '1';
        position_string[1] = '0' + car.position % 10;
    }
    QString s4 = position_string;
    s4.prepend("position:");
    p.drawText(5, 50, s4);

    QString s5 = QString();
    s5.setNum(TIME, 10);
    s5.prepend("TIME:");
    p.drawText(5, 20, s5);
}
