#include "Machine.hpp"
#include "TuringMachine.hpp"

struct Sieve : public Machine
{
    enum State {
        INIT,
        FIND_PREV_PRIME,
        FIND_NEXT_PRIME,
        MOVE_LOCATE_SEQ,
        MOVE_NON_MULTIPLE,
        MOVE_MULTIPLE,
        MOVE_RETURN,
        DIV_LOCATE_FIRST,
        DIV_LOCATE_NEXT,
        DIV_FETCH,
        DIV_INCREMENT,
        CLEANUP,
        HALT,
    } state;

    virtual ~Sieve() {}

    enum Bit {
        MULTIPLE_OR_1 = 1,
        MAYBE_PRIME_OR_1 = 2,
        UNARY_SEQ = 4
    };
    struct Symbol {
        int val;
        Symbol(Bit bit) : val(bit) {}
        Symbol(int val) : val(val) {}
        Symbol(QColor const & c)
        : val((c.red() ? MULTIPLE_OR_1 : 0) | (c.green() ? MAYBE_PRIME_OR_1 : 0) | (c.blue() ? UNARY_SEQ : 0)) {}
        operator int() const { return val; }
        operator QColor() const {
            return QColor(val & MULTIPLE_OR_1 ? 255 : 0,
                          val & MAYBE_PRIME_OR_1 ? 255 : 0,
                          val & UNARY_SEQ ? 255 : 0);
        }
    };

    virtual void reset(std::vector<QColor> & tape)
    {
        int tapeLen = (int)tape.size();
        for (int i = 0; i < tapeLen; ++i) {
            tape[i] = Symbol(MAYBE_PRIME_OR_1);
        }
        state = INIT;
    }

    struct Transition {
        Symbol write;
        Direction dir;
        State nextState;
    };
    Transition eval(Symbol const sym) const
    {
        switch (state) {
            case INIT:
                return Transition{ MULTIPLE_OR_1 | MAYBE_PRIME_OR_1, RIGHT, FIND_NEXT_PRIME };

            // FIND_*: Locate the next prime. It's the one after the previous prime, which
            // is still marked as a multiple.
            case FIND_PREV_PRIME:
                if (sym & MULTIPLE_OR_1)
                    return Transition{ MAYBE_PRIME_OR_1 | UNARY_SEQ, RIGHT, FIND_NEXT_PRIME };
                else
                    return Transition{ sym | UNARY_SEQ, RIGHT, FIND_PREV_PRIME };

            case FIND_NEXT_PRIME:
                if (sym & MAYBE_PRIME_OR_1)
                    return Transition{ MULTIPLE_OR_1 | UNARY_SEQ, RIGHT, MOVE_NON_MULTIPLE };
                else
                    return Transition{ UNARY_SEQ, RIGHT, FIND_NEXT_PRIME };

            // MOVE_*: Push the unary sequence forward, marking multiples of the prime.
            // The prime is not marked as prime temporarily so that cell 1 is distinct.
            case MOVE_LOCATE_SEQ:
                if (sym & UNARY_SEQ)
                    if (sym & MULTIPLE_OR_1)
                        return Transition{ sym & ~UNARY_SEQ, RIGHT, MOVE_MULTIPLE };
                    else
                        return Transition{ sym & ~UNARY_SEQ, RIGHT, MOVE_NON_MULTIPLE };
                else if ((sym & MULTIPLE_OR_1) && (sym & MAYBE_PRIME_OR_1))
                    return Transition{ sym, RIGHT, DIV_LOCATE_FIRST };
                else
                    return Transition{ sym, RIGHT, MOVE_LOCATE_SEQ };

            case MOVE_NON_MULTIPLE:
                if (sym & UNARY_SEQ)
                    return Transition{ sym, RIGHT, MOVE_NON_MULTIPLE };
                else if ((sym & MULTIPLE_OR_1) && (sym & MAYBE_PRIME_OR_1))
                    return Transition{ sym, LEFT, MOVE_RETURN };
                else
                    return Transition{ (sym & MAYBE_PRIME_OR_1) | UNARY_SEQ, LEFT, MOVE_RETURN };

            case MOVE_MULTIPLE:
                if (sym & UNARY_SEQ)
                    return Transition{ sym, RIGHT, MOVE_MULTIPLE };
                else if ((sym & MULTIPLE_OR_1) && (sym & MAYBE_PRIME_OR_1))
                    return Transition{ sym, LEFT, MOVE_RETURN };
                else
                    return Transition{ MULTIPLE_OR_1 | UNARY_SEQ, LEFT, MOVE_RETURN };

            case MOVE_RETURN:
                if (sym & UNARY_SEQ)
                    return Transition{ sym, LEFT, MOVE_RETURN };
                else
                    return Transition{ sym, RIGHT, MOVE_LOCATE_SEQ };

            // DIV_*: Accumulate the marked multiples into a single unary sequence (compute n / p). If
            // the sequence reaches from 1 to the prime, p < sqrt(n). Otherwise, the sieve is complete.
            // The first multiple stays marked so p can be identified later.
            case DIV_LOCATE_FIRST:
                if (sym & MULTIPLE_OR_1)
                    return Transition{ MULTIPLE_OR_1 | MAYBE_PRIME_OR_1 | UNARY_SEQ, RIGHT, DIV_LOCATE_NEXT };
                else
                    return Transition{ sym, RIGHT, DIV_LOCATE_FIRST };

            case DIV_LOCATE_NEXT:
                if ((sym & MULTIPLE_OR_1) && !(sym & UNARY_SEQ))
                    if (sym & MAYBE_PRIME_OR_1)
                        return Transition{ sym, RIGHT, CLEANUP };
                    else
                        return Transition{ 0, LEFT, DIV_FETCH };
                else
                    return Transition{ sym, RIGHT, DIV_LOCATE_NEXT };

            case DIV_FETCH:
                if (sym & UNARY_SEQ)
                    return Transition{ sym, LEFT, DIV_INCREMENT };
                else
                    return Transition{ sym, LEFT, DIV_FETCH };

            case DIV_INCREMENT:
                if (sym & MULTIPLE_OR_1)
                    return Transition{ MULTIPLE_OR_1 | MAYBE_PRIME_OR_1, RIGHT, FIND_PREV_PRIME };
                else if (sym & UNARY_SEQ)
                    return Transition{ sym, LEFT, DIV_INCREMENT };
                else
                    return Transition{ sym | UNARY_SEQ, RIGHT, DIV_LOCATE_NEXT };

            case CLEANUP:
                if (sym == (MULTIPLE_OR_1 | MAYBE_PRIME_OR_1))
                    return Transition{ 0, RIGHT, HALT };
                else
                    return Transition{ sym & MAYBE_PRIME_OR_1, RIGHT, CLEANUP };

            case HALT:
            default:
                return Transition{ sym, LEFT, HALT };
        }
    }

    virtual TapeTransition advance(QColor const & current)
    {
        Transition t = eval(current);
        state = t.nextState;
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
            case INIT: return "I";
            case FIND_PREV_PRIME: return "F1";
            case FIND_NEXT_PRIME: return "F2";
            case MOVE_LOCATE_SEQ: return "M1";
            case MOVE_NON_MULTIPLE: return "M2";
            case MOVE_MULTIPLE: return "M3";
            case MOVE_RETURN: return "M4";
            case DIV_LOCATE_FIRST: return "D1";
            case DIV_LOCATE_NEXT: return "D2";
            case DIV_FETCH: return "D3";
            case DIV_INCREMENT: return "D4";
            case CLEANUP: return ":D";
            case HALT: return "H";
        }
    }

    virtual void renderHead(QPainter & painter, TuringMachine const & tm) const
    {
        (void)tm;
        QFont labelFont;
        qreal fontScale = 7. / QFontMetricsF(labelFont).ascent();
        labelFont.setPointSizeF(labelFont.pointSizeF() * fontScale);

        painter.save();
        painter.setFont(labelFont);
        painter.scale(1. / 4., 1. / 4.);
        painter.setPen(QPen(Qt::black, 0., Qt::SolidLine));
        painter.setBrush(Qt::black);
        painter.drawText(-5, 2, 10, 8, Qt::AlignHCenter, label());
        painter.restore();
    }
};

std::unique_ptr<Machine> createSieve()
{
    return std::unique_ptr<Sieve>(new Sieve());
}

