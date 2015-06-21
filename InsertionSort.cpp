#include "Machine.hpp"
#include "TuringMachine.hpp"

struct InsertionSort : public Machine
{
    enum State {
        SCAN,
        LOCATE,
        INSERT,
        HALT
    } state;
    struct Registers {
        QColor lo, hi, samp;
        int escCtr;
    } r;
    int tapeLen;

    virtual ~InsertionSort() {}

    virtual void reset(int tapeLen, QColor const & current)
    {
        state = SCAN;
        r.lo = r.hi = current;
        r.samp = Qt::black;
        r.escCtr = 0;
        this->tapeLen = tapeLen;
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
                if (r.escCtr == tapeLen) {
                    return Transition{ current, LEFT, HALT, { Qt::black, Qt::black, Qt::black, 0 } };
                }
                else if (hueSep(current, r.lo) + hueSep(r.lo, r.hi) < 1.) {
                    return Transition{ current, LEFT, SCAN, { current, r.hi, Qt::black, r.escCtr + 1 } };
                }
                else {
                    return Transition{ current, RIGHT, LOCATE, { r.lo, r.hi, current, r.escCtr + 1 } };
                }

            case LOCATE:
                if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
                    return Transition{ current, RIGHT, LOCATE, { r.lo, r.hi, r.samp, r.escCtr } };
                }
                else {
                    return Transition{ current, LEFT, INSERT, { r.lo, r.hi, r.samp, r.escCtr } };
                }

            case INSERT:
                if (hueSep(r.lo, current) + hueSep(current, r.samp) < 1.) {
                    return Transition{ r.samp, LEFT, INSERT, { r.lo, r.hi, current, r.escCtr } };
                }
                else {
                    return Transition{ r.samp, LEFT, SCAN, { r.lo, r.hi, Qt::black, r.escCtr } };
                }

            case HALT:
                return Transition{ current, LEFT, HALT, { r.lo, r.hi, r.samp, 0 } };
        }
    }

    virtual TapeTransition advance(QColor const & current)
    {
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

std::unique_ptr<Machine> createInsertionSort()
{
    return std::unique_ptr<InsertionSort>(new InsertionSort());
}

