#ifndef CIRCUITCANVAS_H
#define CIRCUITCANVAS_H

#include <QWidget>
#include <QString>

class QGraphicsScene;
class QGraphicsView;
class QLabel;
class ComponentItem;

class CircuitCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit CircuitCanvas(QWidget *parent = nullptr);

private slots:
    void addComponent();
    void deleteSelected();
    void validateCanvas();
    void startWireConnection();
    void cancelWireConnection();

private:
    QGraphicsScene *scene_;
    QGraphicsView *view_;
    QLabel *status_;

    QString nextType_;
    ComponentItem *wireSource_;
    bool wireMode_;

    void completeWireConnection(ComponentItem *destination);
    void simulateCircuit();
};

#endif