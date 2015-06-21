#include "TuringMachine.hpp"
#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QSlider>

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);
    QMainWindow window;
    QWidget * widget = new QWidget(&window);
    QHBoxLayout * layout = new QHBoxLayout();
    TuringMachine * tm = new TuringMachine(40, widget);
    QSlider * slider = new QSlider(Qt::Vertical, widget);
    slider->setRange(100, 1500);
    slider->setValue(1000);
    slider->setInvertedAppearance(true);
    layout->addWidget(tm);
    layout->addWidget(slider);
    widget->setLayout(layout);
    QObject::connect(slider, SIGNAL(valueChanged(int)), tm, SLOT(setSpeed(int)));
    window.setCentralWidget(widget);
    window.show();
    return app.exec();
}

