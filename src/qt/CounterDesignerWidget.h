#ifndef COUNTERDESIGNERWIDGET_H
#define COUNTERDESIGNERWIDGET_H

#include <QWidget>

class QSpinBox;
class QComboBox;
class QTableWidget;
class QLabel;
class QTextEdit;
class QScrollArea;
class CounterCircuitDiagram;

class CounterDesignerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CounterDesignerWidget(QWidget *parent = nullptr);

private slots:
    void designCounter();

private:
    QSpinBox *bits_;
    QComboBox *mode_;
    QComboBox *direction_;
    QComboBox *flipFlop_;
    QTableWidget *table_;
    QLabel *result_;
    QTextEdit *details_;
    CounterCircuitDiagram *diagram_;
};

#endif