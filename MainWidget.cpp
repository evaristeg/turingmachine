#include "Machine.hpp"
#include "MainWidget.hpp"
#include "ResetDialog.hpp"

static int tapeLen = 40;

MainWidget::MainWidget(QWidget * parent)
: QWidget(parent)
{
    layout = new QGridLayout(this);
    tm = new TuringMachine(createSieve(), tapeLen, this);
    slider = new QSlider(Qt::Vertical, this);
    slider->setRange(2000, 11000);
    slider->setValue(10000);
    slider->setInvertedAppearance(true);
    button = new QPushButton("New", this);
    checkBox = new QCheckBox("Fix tape", this);
    layout->addWidget(tm, 0, 0, 3, 2);
    layout->addWidget(checkBox, 0, 1, 1, 2);
    layout->addWidget(slider, 1, 2, 1, 1);
    layout->addWidget(button, 2, 1, 1, 2);
    layout->setColumnStretch(0, 1);
    layout->setRowStretch(1, 1);
    setLayout(layout);
    QObject::connect(checkBox, SIGNAL(stateChanged(int)), tm, SLOT(setFixTape(int)));
    QObject::connect(slider, SIGNAL(valueChanged(int)), tm, SLOT(setSpeed(int)));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(showResetDialog()));
}

void MainWidget::showResetDialog()
{
    ResetDialog dialog(tapeLen);
    bool wasPaused = tm->pause();
    int tmpTapeLen = dialog.exec();
    if (tmpTapeLen) {
        tapeLen = tmpTapeLen;
        tm->reset(tapeLen);
    }
    if (!wasPaused) {
        tm->unpause();
    }
}

QSize MainWidget::sizeHint() const
{
    QMargins margins = contentsMargins();
    QSize tmSize = tm->sizeHint();
    QSize sliderSize = slider->sizeHint();
    return QSize(tmSize.width() + layout->horizontalSpacing() + sliderSize.width() + margins.left() + margins.right(),
                 tmSize.height() + margins.top() + margins.bottom());
}

