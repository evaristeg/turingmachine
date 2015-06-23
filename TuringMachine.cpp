#include "TuringMachine.hpp"
#include <QPainter>
#include <algorithm>
#include <cmath>
#include <random>
#include <utility>

double const pi = 3.141592653589793238463;
std::mt19937 rng;

TuringMachine::TuringMachine(std::unique_ptr<Machine> && machine, int tapeLen, QWidget * parent)
: QWidget(parent)
, machine(std::move(machine))
, speed(1000)
, paused(false)
{
    rng.seed(time.msecsSinceStartOfDay());
    reset(tapeLen);
    time.start();
}

QSize TuringMachine::sizeHint() const
{
    return QSize(500, 500);
}

void TuringMachine::setSpeed(int ms)
{
    speed = ms;
    if (speed == 1500) {
        pause();
    }
    else {
        unpause();
    }
}

bool TuringMachine::pause()
{
    bool wasPaused = paused;
    paused = true;
    return wasPaused;
}

bool TuringMachine::unpause()
{
    bool wasPaused = paused;
    paused = false;
    oldtime = time.elapsed();
    update();
    return wasPaused;
}

void TuringMachine::reset(int tapeLen)
{
    tape.resize(tapeLen);
    machine->reset(tape);
    pos = oldpos = 0;
    started = false;
}

void TuringMachine::renderBox(QPainter & painter, QColor const & fill) const
{
    QPainterPath box;
    box.addRect(-.5, -1., 1., 1.);
    painter.save();
    painter.setPen(QPen(Qt::black, 0.05, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(fill);
    painter.drawPath(box);
    painter.restore();
}

void TuringMachine::paintEvent(QPaintEvent * event)
{
    (void)event;
    if (!paused) {
        int curtime = time.elapsed();
        if (!started) {
            oldpos = pos;
            progress = 0.8;
            oldtime = curtime;
            started = true;
        }
        progress += float(curtime - oldtime) / speed;
        oldtime = curtime;
    }

    while (!machine->halted()  && progress >= 1.) {
        oldpos = pos;
        TapeTransition t = machine->advance(tape[pos]);
        tape[pos] = t.write;
        if (!machine->halted()) {
            pos = (pos + (t.dir == LEFT ? tape.size() - 1 : 1)) % tape.size();
        }
        progress -= 1.;
    }

    qreal rBegin = 360. * oldpos / tape.size();
    int delta = (pos - oldpos + tape.size() + 1) % tape.size() - 1;
    qreal rDelta = 360. * delta / tape.size();
    qreal interp;
    if (progress < 0.2)
        interp = 0.;
    else if (progress < 0.8)
        interp = (1. - cos((progress - 0.2) * pi / 0.6)) / 2.;
    else
        interp = 1.;
    qreal tapeRot = rBegin + rDelta * interp;

    qreal innerRadius = 1.12 * tape.size() / (2. * pi) + 1.2;
    qreal boundRadius = innerRadius + 1.2;
    QPainterPath box;
    box.addRect(-.5, -1., 1., 1.);
    QPainterPath window;
    window.addRect(-.7, -1.2, 1.4, 1.4);
    window = window.subtracted(box);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int dimension = std::min(width(), height());
    painter.scale(dimension / (2. * boundRadius), dimension / (2. * boundRadius));
    painter.translate(boundRadius, boundRadius);
    painter.setPen(QPen(Qt::black, 0.05, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

    painter.save();
    painter.rotate(-tapeRot);
    for (int i = 0; i < (int)tape.size(); ++i) {
        painter.setBrush(tape[i]);
        painter.translate(0., -innerRadius);
        painter.drawPath(box);
        painter.translate(0., innerRadius);
        painter.rotate(360. / tape.size());
    }
    painter.restore();

    painter.translate(0., -innerRadius);

    painter.save();
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawPath(window);
    painter.restore();
    machine->renderHead(painter, *this);

    if (!paused && !machine->halted()) {
        update();
    }
}

