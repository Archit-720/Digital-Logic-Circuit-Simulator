#ifndef BOOLEANSIMPLIFIERWIDGET_H
#define BOOLEANSIMPLIFIERWIDGET_H

#include <QWidget>

class QSpinBox;
class QLineEdit;
class QLabel;
class QTextEdit;
class QTableWidget;

class BooleanSimplifierWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BooleanSimplifierWidget(QWidget *parent = nullptr);

private slots:
    void simplify();

private:
    QSpinBox *variables_;
    QLineEdit *minterms_;
    QLineEdit *dontCares_;

    QLabel *original_;
    QLabel *result_;
    QLabel *verification_;

    QTextEdit *steps_;
    QTableWidget *truthTable_;
};

#endif
