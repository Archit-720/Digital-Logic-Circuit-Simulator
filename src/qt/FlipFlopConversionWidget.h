#ifndef FLIPFLOPCONVERSIONWIDGET_H
#define FLIPFLOPCONVERSIONWIDGET_H

#include <QWidget>

class QComboBox;
class QTableWidget;
class QTextEdit;
class QLabel;
class QScrollArea;
class FlipFlopCircuitDiagram;

class FlipFlopConversionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlipFlopConversionWidget(QWidget *parent = nullptr);

private slots:
    void convert();

private:
    QComboBox *source_;
    QComboBox *target_;
    QTableWidget *table_;
    QTextEdit *result_;
    QLabel *status_;
    FlipFlopCircuitDiagram *diagram_;
};

#endif