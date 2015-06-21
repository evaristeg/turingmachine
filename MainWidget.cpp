#include "Machine.hpp"
#include "MainWidget.hpp"
#include "ResetDialog.hpp"
#include "TuringMachine.hpp"
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>

MainWidget::MainWidget(QWidget * parent)
: QWidget(parent)
{
    QGridLayout * layout = new QGridLayout(this);
    tm = new TuringMachine(createMergeSort(), 40, this);
    QSlider * slider = new QSlider(Qt::Vertical, this);
    slider->setRange(20, 1500);
    slider->setValue(1000);
    slider->setInvertedAppearance(true);
    QPushButton * button = new QPushButton("New", this);
    layout->addWidget(tm, 0, 0, 2, 2);
    layout->addWidget(slider, 0, 2, 1, 1);
    layout->addWidget(button, 1, 1, 1, 2);
    layout->setColumnStretch(0, 1);
    layout->setRowStretch(0, 1);
    setLayout(layout);
    QObject::connect(slider, SIGNAL(valueChanged(int)), tm, SLOT(setSpeed(int)));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(showResetDialog()));
}

void MainWidget::showResetDialog()
{
    ResetDialog dialog;
    tm->pause();
    int tapeLen = dialog.exec();
    tm->reset(tapeLen);
    tm->unpause();
}

