#ifndef KMAPWIDGET_H
#define KMAPWIDGET_H

#include <QWidget>

class QSpinBox;
class QLineEdit;
class QTableWidget;
class QTextEdit;
class QLabel;

class KMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KMapWidget(QWidget *parent = nullptr);

private slots:
    void solveMap();

private:
    QSpinBox *variables_;
    QLineEdit *minterms_;
    QLineEdit *dontCares_;
    QTableWidget *table_;
    QTextEdit *solution_;
    QLabel *status_;

    void updateMap(const class KMapData& data,
                   const class KMapResult& result);
};

#endif
