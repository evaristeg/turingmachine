#pragma once

#include <QTime>
#include <QWidget>
#include <vector>

enum Direction { LEFT, RIGHT };

struct Registers {
    QColor lo, hi, samp;
};

struct State;

struct Transition {
    QColor write;
    Direction dir;
    State const * nextState;
    Registers registers;
};

struct State {
    virtual Transition eval(QColor const & current, Registers const & r) const = 0;
    virtual char const * label() const = 0;
};


class TuringMachine : public QWidget
{
    Q_OBJECT

public:
    TuringMachine(int tapeLen, QWidget * parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void setSpeed(int ms);
    void pause();
    void unpause();
    void reset(int tapeLen);

protected:
    void paintEvent(QPaintEvent * event) Q_DECL_OVERRIDE;

private:
    std::vector<QColor> tape;
    int pos;
    Registers registers;
    State const * state;

    QTime time;
    int speed;
    bool started;
    bool paused;
    int oldpos;
    int oldtime;
    float progress;
    int escCtr;
};

