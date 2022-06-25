#ifndef TIME_H
#define TIME_H

#include <QThread>

class TimeThread : public QThread{
    Q_OBJECT

public:
    TimeThread();
protected:
    void run();
};

#endif

