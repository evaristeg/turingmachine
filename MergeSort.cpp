#include "Machine.hpp"
#include "TuringMachine.hpp"
#include <algorithm>

// Black version is a little easier to visualize
#define AllowBlackTape() 0

struct MergeSort : public Machine
{
    enum State {
        SCAN,
        LOCATE,
        INSERT,
        FETCH,
        HALT
    } state;
    struct Registers {
        QColor lo, hi, samp;
    } r;
    int tapeLen;
    int escCtr;

    virtual ~MergeSort() {}

    virtual void reset(std::vector<QColor> & tape)
    {
        tapeLen = (int)tape.size();
        for (int i = 0; i < tapeLen; ++i) {
            tape[i] = QColor::fromHslF(qreal(i) / tapeLen, .9, .5);
        }
        std::shuffle(tape.begin(), tape.end(), rng);
        state = SCAN;
        r.lo = r.hi = tape[0];
        r.samp = Qt::black;
        escCtr = 0;
    }

    struct Transition {
        QColor write;
        Direction dir;
        State nextState;
        Registers r;
    };
    Transition eval(QColor const & current) const
    {
        switch (state) {
            default:
            case SCAN:
                if (hueSep(current, r.lo) + hueSep(r.lo, r.hi) < 1.) {
                    return Transition{ current, LEFT, SCAN, { current, r.hi, Qt::black } };
                }
                else {
                    #if AllowBlackTape()
                    return Transition{ Qt::black, RIGHT, LOCATE, { r.lo, current, current } };
                    #else
                    return Transition{ current, RIGHT, LOCATE, { r.lo, current, current } };
                    #endif
                }

            case LOCATE:
                if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
                    return Transition{ current, RIGHT, LOCATE, { r.lo, r.hi, r.samp } };
                }
                else {
                    return Transition{ current, LEFT, INSERT, { r.lo, r.hi, r.samp } };
                }

            case INSERT:
                #if AllowBlackTape()
                if (current != Qt::black) {
                #else
                if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
                #endif
                    return Transition{ r.samp, LEFT, INSERT, { r.lo, r.hi, current } };
                }
                else {
                    return Transition{ r.samp, LEFT, FETCH, { r.lo, r.hi, Qt::black } };
                }

            case FETCH:
                if (hueSep(r.lo, current) + hueSep(current, r.hi) < 1.) {
                    #if AllowBlackTape()
                    return Transition{ Qt::black, RIGHT, LOCATE, { r.lo, current, current } };
                    #else
                    return Transition{ current, RIGHT, LOCATE, { r.lo, current, current } };
                    #endif
                }
                else {
                    return Transition{ current, LEFT, SCAN, { current, current, Qt::black } };
                }

            case HALT:
                return Transition{ current, LEFT, HALT, { r.lo, r.hi, r.samp } };
        }
    }

    virtual TapeTransition advance(QColor const & current)
    {
        if (state == SCAN) { escCtr++; } else { escCtr = 0; }
        if (escCtr == tapeLen) { state = HALT; }
        Transition t = eval(current);
        state = t.nextState;
        r = t.r;
        return TapeTransition{ t.write, t.dir };
    }

    virtual bool halted() const
    {
        return state == HALT;
    }

    char const * label() const
    {
        switch (state) {
            default:
            case SCAN:   return "S";
            case LOCATE: return "L";
            case INSERT: return "I";
            case FETCH:  return "F";
            case HALT:   return "H";
        }
    }

    virtual void renderHead(QPainter & painter, TuringMachine const & tm) const
    {
        QFont labelFont;
        qreal fontScale = 10. / QFontMetricsF(labelFont).ascent();
        labelFont.setPointSizeF(labelFont.pointSizeF() * fontScale);

        painter.save();
        painter.setFont(labelFont);
        painter.scale(1. / 4., 1. / 4.);
        painter.setPen(QPen(Qt::black, 0., Qt::SolidLine));
        painter.setBrush(Qt::black);
        painter.drawText(-5, 2, 10, 10, Qt::AlignHCenter, label());
        painter.restore();

        painter.save();
        painter.translate(-1.5, 4.5);
        tm.renderBox(painter, r.lo);
        painter.translate(1.5, 0.);
        tm.renderBox(painter, r.samp);
        painter.translate(1.5, 0.);
        tm.renderBox(painter, r.hi);
        painter.restore();
    }
};

std::unique_ptr<Machine> createMergeSort()
{
    return std::unique_ptr<MergeSort>(new MergeSort());
}

