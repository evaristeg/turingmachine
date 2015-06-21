#pragma once

#include <QPainter>
#include <cmath>
#include <memory>

class TuringMachine;

enum Direction { LEFT, RIGHT };

struct TapeTransition
{
    QColor write;
    Direction dir;
};

struct Machine
{
    virtual ~Machine() {}
    virtual void reset(int tapeLen, QColor const & current) = 0;
    virtual TapeTransition advance(QColor const & current) = 0;
    virtual void renderHead(QPainter & painter, TuringMachine const & tm) const = 0;
    virtual bool halted() const = 0;
};

std::unique_ptr<Machine> createInsertionSort();
std::unique_ptr<Machine> createMergeSort();

inline float hueSep(QColor const & a, QColor const & b)
{
    return fmod(b.hslHueF() - a.hslHueF() + 1., 1.);
}

