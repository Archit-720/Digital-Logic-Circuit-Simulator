#ifndef COMPONENTITEM_H
#define COMPONENTITEM_H

#include <QGraphicsObject>
#include <QString>
#include <QPointF>
#include <QList>

class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QVariant;
class QGraphicsSceneMouseEvent;
class WireItem;

class ComponentItem : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit ComponentItem(const QString &type, QGraphicsItem *parent = nullptr);

    QList<WireItem *> wires() const { return wires_; }

    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    QString componentType() const;
    int inputPinCount() const;
    QPointF inputPinPosition(int pinIndex) const;
    QPointF outputPinPosition() const;

    int logicValue() const;
    void setLogicValue(int value);
    void toggleInput();

    void addWire(WireItem *wire);
    void removeWire(WireItem *wire);
    void updateConnectedWires();

signals:
    void inputToggled();

protected:
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    QString type_;
    QList<WireItem *> wires_;
    int logicValue_ = 0;
};

#endif