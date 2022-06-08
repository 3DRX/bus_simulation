#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "src/main.h"

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <cstdio>

#define PI 3.1415926535897932384626

extern ENVIRONMENT env;

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent ),
      ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    this->setWindowTitle( "Bus-simulation" );
    // 设置窗口、画布
    // TODO: 改大小后分辨率很迷
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
    // the sations
    // TODO: 画车站
}

void MainWindow::paintStations( void )
{
    QPainter p( &pix );
    for ( int i = 0; i < env.TOTAL_STATION * env.DISTANCE; i++ ) {
        if ( ( i % 5 ) != 0 ) { // 这里不应该是5，取决于DISTANCE
            // draw small circle
        }
        else {
            // draw big circle
        }
        p.rotate( 6.0 );
    }
}
