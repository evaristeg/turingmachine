#pragma once

#include "TuringMachine.hpp"
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QWidget>

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget * parent = 0);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void showResetDialog();

private:
    QGridLayout * layout;
    TuringMachine * tm;
    QSlider * slider;
    QPushButton * button;
    QCheckBox * checkBox;
};

