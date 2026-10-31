#ifndef WIREITEM_H
#define WIREITEM_H

#include <QGraphicsPathItem>
#include <QPointer>

class ComponentItem;

class WireItem : public QGraphicsPathItem
{
public:
    WireItem(ComponentItem *source,
             ComponentItem *destination,
             int inputPin,
             QGraphicsItem *parent = nullptr);

    void updatePath();
    void setLogicValue(int value);
    int logicValue() const;

    ComponentItem *sourceComponent() const;
    ComponentItem *destinationComponent() const;
    int destinationInputPin() const;

private:
    QPointer<QObject> sourceObject_;
    QPointer<QObject> destinationObject_;

    ComponentItem *source_;
    ComponentItem *destination_;
    int inputPin_;
    int logicValue_ = -1;
};

#endif
