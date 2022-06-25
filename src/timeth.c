#include "timeth.h"

TimeThread::TimeThread()
{
}

void TimeThread::run()
{
    // TODO: 在这里进行时间的更新（用于动画的固定帧率播放）
    // 参考程序如下：
    
    //struct timeval tp;
    //long long int ms;
    //long long int lastTime;
    //int fps = 30;
    //int frame = 0;
    //gettimeofday(&tp, NULL);
    //lastTime = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    //while (1) {
    //gettimeofday(&tp, NULL);
    //ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    //if (ms == lastTime + 33) {
    //lastTime = ms;
    //printf("frame\n");
    //frame++;
    //}
    //if (frame == fps) {
    //frame = 0;
    //printf("second\n");
    //}
    //}
    //return 0;
}
