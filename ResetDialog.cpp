#include "ResetDialog.hpp"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>

ResetDialog::ResetDialog(QWidget * parent)
: QDialog(parent)
{
    slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(20);
    slider->setMaximum(200);
    slider->setValue(40);
    QPushButton * button = new QPushButton("Go", this);
    QLabel * sizeLabel = new QLabel("40", this);
    sizeLabel->setAlignment(Qt::AlignRight);
    QLabel * instructions = new QLabel("Select tape size:");
    QObject::connect(slider, SIGNAL(valueChanged(int)), sizeLabel, SLOT(setNum(int)));
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(finish()));
    QGridLayout * layout = new QGridLayout(this);
    layout->addWidget(instructions, 0, 0, 1, 3, Qt::AlignLeft);
    layout->addWidget(sizeLabel, 1, 0, 1, 1);
    layout->addWidget(slider, 1, 1, 1, 2);
    layout->addWidget(button, 2, 2, 1, 1);
    layout->setColumnStretch(1, 1);
    setLayout(layout);
}

QSize ResetDialog::sizeHint() const
{
    return QSize(300, 20);
}

void ResetDialog::finish()
{
    done(slider->value());
}

