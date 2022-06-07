#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDialog>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>

MainWindow::MainWindow( QWidget* parent )
    : QMainWindow( parent ),
      ui( new Ui::MainWindow )
{
    ui->setupUi( this );
    this->setWindowTitle("Bus-simulation");
}

MainWindow::~MainWindow()
{
    delete ui;
}

