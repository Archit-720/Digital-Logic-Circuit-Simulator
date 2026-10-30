#ifndef TRUTHTABLEWIDGET_H
#define TRUTHTABLEWIDGET_H

#include <QWidget>

class QLineEdit;
class QSpinBox;
class QTableWidget;
class QLabel;
class QTextEdit;

class TruthTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TruthTableWidget(QWidget *parent = nullptr);

private slots:
    void generateTable();

private:
    QLineEdit *expression_;
    QSpinBox *variables_;
    QTableWidget *table_;
    QLabel *status_;
    QLabel *summary_;
    QTextEdit *steps_;
};

#endif
