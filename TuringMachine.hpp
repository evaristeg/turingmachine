#pragma once

#include "Machine.hpp"
#include <QTime>
#include <QWidget>
#include <memory>
#include <vector>

class TuringMachine : public QWidget
{
    Q_OBJECT

public:
    TuringMachine(std::unique_ptr<Machine> && machine, int tapeLen, QWidget * parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    void renderBox(QPainter & painter, QColor const & fill) const;

public slots:
    void setSpeed(int ms);
    void pause();
    void unpause();
    void reset(int tapeLen);

protected:
    void paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;

private:
    std::unique_ptr<Machine> machine;
    std::vector<QColor> tape;
    int pos;

    QTime time;
    int speed;
    bool started;
    bool paused;
    int oldpos;
    int oldtime;
    float progress;
};

