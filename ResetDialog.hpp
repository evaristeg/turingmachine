#pragma once

#include <QAbstractSlider>
#include <QDialog>

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    ResetDialog(QWidget * parent = 0);
    QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void finish();

private:
    QAbstractSlider * slider;
};

