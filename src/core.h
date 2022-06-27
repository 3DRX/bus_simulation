#ifndef CORE_H
#define CORE_H

#include <QThread>

class CoreThread : public QThread {
    Q_OBJECT

  public:
    CoreThread();

  protected:
    void run();
};

#endif
