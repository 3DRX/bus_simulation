#include "main.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "core.h"

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <cmath>
#include <cstdio>

#define PI 3.1415926535897932384626

extern ENVIRONMENT env;
extern GLOB        global;

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent ),
      ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    this->setWindowTitle( "Bus-simulation" );
    // 设置窗口、画布
    resize( 1024, 768 );
    pix = QPixmap( 1024, 700 );
    pix.fill( Qt::white );
    // 创建两个按钮
    button_next = new QPushButton( this );
    button_next->setText( tr( "next" ) );
    button_next->move( 920, 701 );
    connect( button_next, SIGNAL( clicked() ), this, SLOT( next() ) );
    button_previous = new QPushButton( this );
    button_previous->setText( tr( "previous" ) );
    button_previous->move( 0, 701 );
    connect( button_previous, SIGNAL( clicked() ), this, SLOT( previous() ) );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::next()
{
    printf( "button_next pressed\n" );
    global.ifWait = false;
}

void MainWindow::previous()
{
    printf( "button_previous pressed\n" );
}

void MainWindow::paintEvent( QPaintEvent* )
{
    // 主绘图函数
    paintBackground();
    paintStations();
    QPainter painter( this );
    painter.drawPixmap( 0, 0, pix );
}

void MainWindow::paintBackground( void )
{
    // the big circle
    // center: (512, 350)    radious: 300
    QPainter p( &pix );
    p.setRenderHint( QPainter::Antialiasing );
    p.setPen( QPen( Qt::black, 6 ) );
    QRectF rectangle( 512 - 300, 350 - 300, 600, 600 );
    p.drawEllipse( rectangle );
}

void MainWindow::paintStations( void )
{
    double   theta = ( double )360 / ( env.TOTAL_STATION * env.DISTANCE );
    QPainter p( &pix );
    p.setRenderHint( QPainter::Antialiasing );
    p.translate( 512, 350 );
    for ( int i = 0; i < ( env.TOTAL_STATION * env.DISTANCE ); i++ ) {
        double angle = theta * i;
        if ( ( i % env.DISTANCE ) != 0 ) {
            // draw small circle
            QRectF rectangle( 290, -10, 20, 20 );
            p.setPen( Qt::black );
            p.setBrush( Qt::black );
            p.drawEllipse( rectangle );
        }
        else {
            char a[3];
            a[1] = '\0';
            a[2] = '\0';
            if ( i == 0 ) {
                a[0] = '1';
            }
            else { // 咱就当他最多是10站吧
                if (i/env.DISTANCE == 9) {
                    a[0] = '1';
                    a[1] = '0';
                }
                else {
                    a[0] = '1' + i / env.DISTANCE;
                }
            }
            // draw big circle
            QRectF rectangle( 280, -20, 40, 40 );
            p.setPen( Qt::black );
            p.setBrush( Qt::black );
            p.drawEllipse( rectangle );
            p.save();
            // draw text
            // 转回原来的角度，使数字是正的
            p.rotate( ( double )( 360 - ( i * ( ( double )360 / ( env.TOTAL_STATION * env.DISTANCE ) ) ) ) );
            QFont font( "arial", 18, QFont::Bold, false );
            p.setFont( font );
            p.setPen( Qt::white );
            // 手动极坐标，+5-5是为了让字（而不是字的左下角）在圆中央
            p.drawText( ( 300 * std::cos( angle * PI / 180 ) ) - 5,
                        ( 300 * std::sin( angle * PI / 180 ) ) + 5, tr(a) );
            p.restore();
        }
        p.rotate( ( double )360 / ( env.TOTAL_STATION * env.DISTANCE ) );
    }
}
