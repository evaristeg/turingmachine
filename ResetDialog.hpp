#pragma once

#include <QAbstractSlider>
#include <QDialog>

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    ResetDialog(int presetSize, QWidget * parent = 0);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

public slots:
    void finish();

private:
    QAbstractSlider * slider;
};

