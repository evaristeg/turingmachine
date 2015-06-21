#include "TuringMachine.hpp"
#include <QPainter>
#include <algorithm>
#include <cmath>

// Black version is a little easier to visualize
#define AllowBlackTape() 0

double const pi = 3.141592653589793238463;

State const * scan;
State const * locate;
State const * insert;
State const * fetch;
State const * halt;

static float hueSep(QColor const & a, QColor const & b)
{
    return fmod(b.hslHueF() - a.hslHueF() + 1., 1.);
}

class Scan : public State
{
    static Scan const instance;
    Scan() { scan = this; }
public:
    virtual Transition eval(QColor const & current, Registers const & r) const
    {
        if (hueSep(current, r.lo) + hueSep(r.lo, r.hi) < 1.) {
            Transition t{ current, LEFT, scan, { current, r.hi, Qt::black } };
            return t;
        }
        else {
            #if AllowBlackTape()
            Transition t{ Qt::black, RIGHT, locate, { r.lo, current, current } };
            #else
            Transition t{ current, RIGHT, locate, { r.lo, current, current } };
            #endif
            return t;
        } 
    }
    virtual char const * label() const { return "S"; }
};

class Locate : public State
{
    static Locate const instance;
    Locate() { locate = this; }
public:
    virtual Transition eval(QColor const & current, Registers const & r) const
    {
        if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
            Transition t{ current, RIGHT, locate, { r.lo, r.hi, r.samp } };
            return t;
        }
        else {
            Transition t{ current, LEFT, insert, { r.lo, r.hi, r.samp } };
            return t;
        }
    }
    virtual char const * label() const { return "L"; }
};

class Insert : public State
{
    static Insert const instance;
    Insert() { insert = this; }
public:
    virtual Transition eval(QColor const & current, Registers const & r) const
    {
        #if AllowBlackTape()
        if (current != Qt::black) {
        #else
        if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
        #endif
            Transition t{ r.samp, LEFT, insert, { r.lo, r.hi, current } };
            return t;
        }
        else {
            Transition t{ r.samp, LEFT, fetch, { r.lo, r.hi, Qt::black } };
            return t;
        }
    }
    virtual char const * label() const { return "I"; }
};

class Fetch : public State
{
    static Fetch const instance;
    Fetch() { fetch = this; }
public:
    virtual Transition eval(QColor const & current, Registers const & r) const
    {
        if (hueSep(r.lo, current) + hueSep(current, r.hi) < 1.) {
            #if AllowBlackTape()
            Transition t{ Qt::black, RIGHT, locate, { r.lo, current, current } };
            #else
            Transition t{ current, RIGHT, locate, { r.lo, current, current } };
            #endif
            return t;
        }
        else {
            Transition t{ current, LEFT, scan, { current, current, Qt::black } };
            return t;
        }
    }
    virtual char const * label() const { return "F"; }
};

class Halt : public State
{
    static Halt const instance;
    Halt() { halt = this; }
public:
    virtual Transition eval(QColor const & current, Registers const & r) const
    {
        Transition t{ current, LEFT, halt, { r.lo, r.hi, r.samp } };
        return t;
    }
    virtual char const * label() const { return "H"; }
};

Scan const Scan::instance;
Locate const Locate::instance;
Insert const Insert::instance;
Fetch const Fetch::instance;
Halt const Halt::instance;


TuringMachine::TuringMachine(int tapeLen, QWidget * parent)
: QWidget(parent)
, speed(1000)
, paused(false)
{
    qsrand(time.msecsSinceStartOfDay());
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

void TuringMachine::pause()
{
    paused = true;
}

void TuringMachine::unpause()
{
    paused = false;
    oldtime = time.elapsed();
    update();
}

void TuringMachine::reset(int tapeLen)
{
    tape.resize(tapeLen);
    for (int i = 0; i < tapeLen; ++i) {
        tape[i] = QColor::fromHslF(qreal(qrand()) / RAND_MAX, .9, .5);
    }
    pos = oldpos = escCtr = 0;
    state = scan;
    registers.lo = registers.hi = tape[pos];
    started = false;
}

void TuringMachine::paintEvent(QPaintEvent * event)
{
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

    while (state != halt && progress >= 1.) {
        if (state == scan) {
            escCtr++;
            if (escCtr == tape.size()) {
                state = halt;
                break;
            }
        }
        else {
            escCtr = 0;
        }
        oldpos = pos;
        Transition t = state->eval(tape[pos], registers);
        tape[pos] = t.write;
        pos = (pos + (t.dir == LEFT ? tape.size() - 1 : 1)) % tape.size();
        registers = t.registers;
        state = t.nextState;
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

    qreal boxWid = 2. * pi / (1.2 * tape.size());
    boxWid = boxWid / (1. + boxWid);
    QPainterPath box;
    box.addRect(-.5 * boxWid, -.5 * boxWid, boxWid, boxWid);
    QPainterPath window;
    window.addRect(-.7 * boxWid, -.7 * boxWid, 1.4 * boxWid, 1.4 * boxWid);
    window = window.subtracted(box);
    QFont labelFont;
    qreal fontScale = 10. / QFontMetricsF(labelFont).ascent();
    labelFont.setPointSizeF(labelFont.pointSizeF() * fontScale);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int dimension = std::min(width(), height());
    painter.scale(dimension / 2.2, dimension / 2.2);
    painter.translate(1.1, 1.1);
    painter.setPen(QPen(Qt::black, boxWid * 0.05, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

    painter.save();
    painter.rotate(-tapeRot);
    for (int i = 0; i < tape.size(); ++i) {
        painter.setBrush(tape[i]);
        painter.translate(0., -1.);
        painter.drawPath(box);
        painter.translate(0., 1.);
        painter.rotate(360. / tape.size());
    }
    painter.restore();

    painter.save();
    painter.setFont(labelFont);
    painter.translate(0., -1.);
    painter.scale(boxWid / 4., boxWid / 4.);
    painter.setPen(QPen(Qt::black, 0., Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.setBrush(Qt::black);
    painter.drawText(-5, 4, 10, 10, Qt::AlignHCenter, state->label());
    painter.restore();

    painter.save();
    painter.translate(0., -1.);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::darkGray);
    painter.drawPath(window);
    painter.restore();

    painter.save();
    painter.translate(-1.5 * boxWid, -1. + 4.5 * boxWid);
    painter.setBrush(registers.lo);
    painter.drawPath(box);
    painter.translate(1.5 * boxWid, 0.);
    painter.setBrush(registers.samp);
    painter.drawPath(box);
    painter.translate(1.5 * boxWid, 0.);
    painter.setBrush(registers.hi);
    painter.drawPath(box);
    painter.restore();

    if (!paused && state != halt) {
        update();
    }
}

