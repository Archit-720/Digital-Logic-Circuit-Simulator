#include "ComponentItem.h"
#include "WireItem.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include <QFont>
#include <QPen>
#include <QBrush>
#include <QWidget>

ComponentItem::ComponentItem(const QString &type, QGraphicsItem *parent)
    : QGraphicsObject(parent),
      type_(type.trimmed().toUpper())
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1);
}

QRectF ComponentItem::boundingRect() const
{
    return QRectF(-6, -6, 132, 88);
}

QString ComponentItem::componentType() const
{
    return type_;
}

int ComponentItem::logicValue() const
{
    return logicValue_;
}

void ComponentItem::setLogicValue(int value)
{
    value = value ? 1 : 0;

    if (logicValue_ == value)
        return;

    logicValue_ = value;
    update();
    updateConnectedWires();
}

void ComponentItem::toggleInput()
{
    if (type_ != "INPUT")
        return;

    logicValue_ = logicValue_ ? 0 : 1;
    update();
    updateConnectedWires();
    emit inputToggled();
}

int ComponentItem::inputPinCount() const
{
    if (type_ == "INPUT")
        return 0;

    if (type_ == "OUTPUT" || type_ == "LED" || type_ == "NOT")
        return 1;

    if (type_ == "D FLIP-FLOP")
        return 2;

    return 2;
}

QPointF ComponentItem::inputPinPosition(int pinIndex) const
{
    if (pinIndex < 0 || pinIndex >= inputPinCount())
        return QPointF();

    if (type_ == "OUTPUT" || type_ == "LED" || type_ == "NOT")
        return QPointF(0, 38);

    return pinIndex == 0 ? QPointF(0, 25) : QPointF(0, 51);
}

QPointF ComponentItem::outputPinPosition() const
{
    return QPointF(120, 38);
}

void ComponentItem::paint(
    QPainter *painter,
    const QStyleOptionGraphicsItem *,
    QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    const QColor outline = isSelected()
                               ? QColor(255, 190, 80)
                               : QColor(115, 145, 180);

    const QColor fill = isSelected()
                            ? QColor(61, 66, 78)
                            : QColor(38, 45, 58);

    const QColor textColor(235, 241, 250);
    const QColor pinColor(100, 210, 190);

    painter->setPen(QPen(outline, isSelected() ? 2.6 : 1.8));
    painter->setBrush(fill);
    painter->setFont(QFont("Segoe UI", 9, QFont::DemiBold));

    const QRectF body(16, 12, 88, 52);

    if (type_ == "INPUT")
    {
        painter->drawRoundedRect(body, 9, 9);
        painter->setPen(textColor);
        painter->drawText(body, Qt::AlignCenter,
                          QString("INPUT\n%1").arg(logicValue_));
    }
    else if (type_ == "OUTPUT" || type_ == "LED")
    {
        painter->drawRoundedRect(body, 9, 9);

        if (type_ == "LED")
        {
            painter->setPen(Qt::NoPen);
            painter->setBrush(logicValue_ ? QColor(65, 240, 110)
                                         : QColor(75, 75, 85));
            painter->drawEllipse(QPointF(34, 38), 8, 8);

            painter->setPen(textColor);
            painter->drawText(QRectF(43, 12, 58, 52),
                              Qt::AlignCenter,
                              QString("LED\n%1").arg(logicValue_));
        }
        else
        {
            painter->setPen(textColor);
            painter->drawText(body, Qt::AlignCenter,
                              QString("OUTPUT\n%1").arg(logicValue_));
        }
    }
    else if (type_ == "NOT")
    {
        QPainterPath gate;
        gate.moveTo(22, 15);
        gate.lineTo(22, 61);
        gate.lineTo(82, 38);
        gate.closeSubpath();

        painter->drawPath(gate);
        painter->drawEllipse(QPointF(91, 38), 5, 5);
        painter->setPen(textColor);
        painter->drawText(QRectF(20, 12, 74, 52),
                          Qt::AlignCenter, "NOT");
    }
    else if (type_ == "AND" || type_ == "NAND")
    {
        QPainterPath gate;
        gate.moveTo(22, 15);
        gate.lineTo(53, 15);
        gate.arcTo(QRectF(30, 15, 46, 46), 90, -180);
        gate.lineTo(22, 61);
        gate.closeSubpath();

        painter->drawPath(gate);

        if (type_ == "NAND")
            painter->drawEllipse(QPointF(84, 38), 5, 5);

        painter->setPen(textColor);
        painter->drawText(QRectF(25, 18, 48, 40),
                          Qt::AlignCenter, type_);
    }
    else if (type_ == "OR" || type_ == "XOR" ||
             type_ == "NOR" || type_ == "XNOR")
    {
        QPainterPath gate;
        gate.moveTo(22, 15);
        gate.quadTo(42, 38, 22, 61);
        gate.quadTo(62, 59, 82, 38);
        gate.quadTo(62, 17, 22, 15);
        gate.closeSubpath();

        if (type_ == "XOR" || type_ == "XNOR")
        {
            QPainterPath extra;
            extra.moveTo(14, 15);
            extra.quadTo(34, 38, 14, 61);
            painter->drawPath(extra);
        }

        painter->drawPath(gate);

        if (type_ == "NOR" || type_ == "XNOR")
            painter->drawEllipse(QPointF(90, 38), 5, 5);

        painter->setPen(textColor);
        painter->drawText(QRectF(28, 18, 47, 40),
                          Qt::AlignCenter, type_);
    }
    else if (type_ == "D FLIP-FLOP")
    {
        painter->drawRoundedRect(body, 5, 5);
        painter->setPen(textColor);
        painter->drawText(body, Qt::AlignCenter, "D FF");
        painter->drawText(QRectF(19, 13, 25, 20),
                          Qt::AlignLeft, "D");
        painter->drawText(QRectF(19, 43, 35, 18),
                          Qt::AlignLeft, "CLK");
        painter->drawText(QRectF(78, 13, 22, 20),
                          Qt::AlignRight, "Q");
    }
    else
    {
        painter->drawRoundedRect(body, 9, 9);
        painter->setPen(textColor);
        painter->drawText(body, Qt::AlignCenter, type_);
    }

    painter->setPen(QPen(pinColor, 1.8));
    painter->setBrush(pinColor);

    for (int i = 0; i < inputPinCount(); ++i)
    {
        QPointF pin = inputPinPosition(i);
        painter->drawLine(pin, pin + QPointF(16, 0));
        painter->drawEllipse(pin, 3.1, 3.1);
    }

    QPointF outputPin = outputPinPosition();
    painter->drawLine(outputPin - QPointF(16, 0), outputPin);
    painter->drawEllipse(outputPin, 3.1, 3.1);
}

void ComponentItem::addWire(WireItem *wire)
{
    if (wire && !wires_.contains(wire))
        wires_.append(wire);
}

void ComponentItem::removeWire(WireItem *wire)
{
    wires_.removeAll(wire);
}

void ComponentItem::updateConnectedWires()
{
    for (WireItem *wire : wires_)
    {
        if (wire)
            wire->updatePath();
    }
}

QVariant ComponentItem::itemChange(
    GraphicsItemChange change,
    const QVariant &value)
{
    QVariant result = QGraphicsObject::itemChange(change, value);

    if (change == ItemPositionHasChanged)
        updateConnectedWires();

    return result;
}

void ComponentItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (type_ == "INPUT" && event->button() == Qt::LeftButton)
    {
        toggleInput();
        event->accept();
        return;
    }

    QGraphicsObject::mousePressEvent(event);
}