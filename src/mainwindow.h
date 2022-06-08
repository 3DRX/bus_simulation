#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( QWidget* parent = nullptr );
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    QPixmap         pix;
    QPushButton*    button_next;
    QPushButton*    button_previous;

    //=====private functions=====
    
    /**绘制大圆轨道和小圆点车站
     */
    void paintBackground(void);
    void paintStations(void);

protected:
    void paintEvent( QPaintEvent* );

private slots:
    // 按next按钮触发的回调函数
    void next();
    // 按previous按钮触发的回调函数
    void previous();
};
#endif // MAINWINDOW_H
