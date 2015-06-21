#pragma once

#include "TuringMachine.hpp"
#include <QWidget>

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget * parent = 0);

public slots:
    void showResetDialog();

private:
    TuringMachine * tm;
};

