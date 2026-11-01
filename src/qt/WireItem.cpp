#include "WireItem.h"
#include "ComponentItem.h"

#include <QPainterPath>
#include <QPen>
#include <QGraphicsItem>
#include <QColor>

WireItem::WireItem(
    ComponentItem *source,
    ComponentItem *destination,
    int inputPin,
    QGraphicsItem *parent)
    : QGraphicsPathItem(parent),
      sourceObject_(source),
      destinationObject_(destination),
      source_(source),
      destination_(destination),
      inputPin_(inputPin)
{
    setPen(QPen(QColor(130, 145, 160), 2.5));
    setZValue(-1);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    updatePath();
}

void WireItem::updatePath()
{
    if (!source_ || !destination_)
    {
        setPath(QPainterPath());
        return;
    }

    QPointF start = source_->mapToScene(source_->outputPinPosition());
    QPointF end = destination_->mapToScene(
        destination_->inputPinPosition(inputPin_));

    QPainterPath wire;
    wire.moveTo(start);

    qreal middleX = (start.x() + end.x()) / 2.0;
    wire.lineTo(middleX, start.y());
    wire.lineTo(middleX, end.y());
    wire.lineTo(end);

    setPath(wire);
}

void WireItem::setLogicValue(int value)
{
    if (value < 0)
    {
        logicValue_ = -1;
        setPen(QPen(QColor(130, 145, 160), 2.5));
        return;
    }

    logicValue_ = value ? 1 : 0;

    const QColor color = logicValue_
                             ? QColor(65, 240, 110)
                             : QColor(240, 95, 95);

    setPen(QPen(color, 3.0));
}

int WireItem::logicValue() const
{
    return logicValue_;
}

ComponentItem *WireItem::sourceComponent() const
{
    return source_;
}

ComponentItem *WireItem::destinationComponent() const
{
    return destination_;
}

int WireItem::destinationInputPin() const
{
    return inputPin_;
}
