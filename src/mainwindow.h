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
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    QPixmap pix;
    QPushButton* button_start;
    QPushButton* button_stop;

    //=====private functions=====

    /**绘制大圆轨道和小圆点车站
     */
    void paintBackground(void);
    void paintStations(void);

    /**绘制公交车
     */
    void paintBus(void);
    void moveBus(void);

    /**绘制输出区和指示灯
     */
    void paintoutput(void);
    void paintlight(void);

protected:
    void paintEvent(QPaintEvent*);

private slots:
    // 按restart按钮触发的回调函数
    void restartPressed();
    // 按start按钮触发的回调函数
    void startPressed();
    // 按stop按钮触发的回调函数
    void stopPressed();
};
#endif // MAINWINDOW_H
