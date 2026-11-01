#ifndef NUMBERCONVERTERWIDGET_H
#define NUMBERCONVERTERWIDGET_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QLabel;
class QTextEdit;

class NumberConverterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NumberConverterWidget(QWidget *parent = nullptr);

private slots:
    void convertNumber();

private:
    QLineEdit *number_;
    QComboBox *fromBase_;
    QComboBox *toBase_;
    QLabel *result_;
    QTextEdit *steps_;
};

#endif