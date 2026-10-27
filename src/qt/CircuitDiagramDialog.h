#ifndef CIRCUITDIAGRAMDIALOG_H
#define CIRCUITDIAGRAMDIALOG_H

#include <QDialog>
#include <QString>

class QGraphicsScene;
class QGraphicsView;

class CircuitDiagramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CircuitDiagramDialog(
        const QString &expression,
        QWidget *parent = nullptr);

private:
    QGraphicsScene *scene_;
    QGraphicsView *view_;

    void drawCircuit(const QString &expression);
};

#endif
