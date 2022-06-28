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

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private:
    Ui::MainWindow *ui;
    QPixmap pix;
    QPushButton *button_next;
    QPushButton *button_previous;
    QPushButton *button_clockwise;
    QPushButton *button_counterclockwise;
    QPushButton *button_stop;

    //=====private functions=====

    /**绘制大圆轨道和小圆点车站
     */
    void paintBackground(void);
    void paintStations(void);

    /**绘制公交车
     */
    void paintBus(void);
    void moveBus(void);

    /**绘制输出区
     */
    void paintoutput(void);


  protected:
    void paintEvent(QPaintEvent *);

  private slots:
    // 按next按钮触发的回调函数
    void next();
    // 按previous按钮触发的回调函数
    void previous();
    // 按clockwise按钮出发的回调函数
    void clockwisePressed();
    // 按counterclockwise按钮出发的回调函数
    void counterclockwisePressed();
    // 按stop按钮触发的回调函数
    void stopPressed();
};
#endif // MAINWINDOW_H
