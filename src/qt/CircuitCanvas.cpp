#include "CircuitCanvas.h"
#include "ComponentItem.h"
#include "WireItem.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainter>
#include <QList>
#include <QSet>
#include <functional>

class CircuitGraphicsView : public QGraphicsView
{
public:
    using QGraphicsView::QGraphicsView;

    std::function<bool(ComponentItem *)> componentClicked;

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && componentClicked)
        {
            QGraphicsItem *item = itemAt(event->position().toPoint());
            auto *component = dynamic_cast<ComponentItem *>(item);

            if (component && componentClicked(component))
            {
                event->accept();
                return;
            }
        }

        QGraphicsView::mousePressEvent(event);
    }
};

CircuitCanvas::CircuitCanvas(QWidget *parent)
    : QWidget(parent),
      scene_(new QGraphicsScene(this)),
      view_(new CircuitGraphicsView(scene_, this)),
      status_(new QLabel("Select a component and click Add component.", this)),
      nextType_("INPUT"),
      wireSource_(nullptr),
      wireMode_(false)
{
    auto *layout = new QVBoxLayout(this);
    auto *bar = new QHBoxLayout;

    auto *types = new QComboBox(this);
    types->addItems({
        "INPUT", "OUTPUT", "AND", "OR", "NOT",
        "NAND", "NOR", "XOR", "XNOR", "LED", "D Flip-Flop"
    });

    auto *add = new QPushButton("Add component", this);
    auto *connectWire = new QPushButton("Connect wires", this);
    auto *cancelWire = new QPushButton("Cancel connection", this);
    auto *remove = new QPushButton("Delete selected", this);
    auto *validate = new QPushButton("Validate workspace", this);

    bar->addWidget(types);
    bar->addWidget(add);
    bar->addWidget(connectWire);
    bar->addWidget(cancelWire);
    bar->addWidget(remove);
    bar->addWidget(validate);

    layout->addLayout(bar);
    layout->addWidget(view_);
    layout->addWidget(status_);

    scene_->setSceneRect(0, 0, 1800, 1100);

    view_->setMinimumHeight(550);
    view_->setRenderHint(QPainter::Antialiasing);
    view_->setDragMode(QGraphicsView::RubberBandDrag);

    connect(types, &QComboBox::currentTextChanged,
            this, [this](const QString &type)
    {
        nextType_ = type.trimmed().toUpper();
    });

    connect(add, &QPushButton::clicked, this, [this, types]()
    {
        nextType_ = types->currentText().trimmed().toUpper();
        addComponent();
    });

    connect(connectWire, &QPushButton::clicked,
            this, &CircuitCanvas::startWireConnection);

    connect(cancelWire, &QPushButton::clicked,
            this, &CircuitCanvas::cancelWireConnection);

    connect(remove, &QPushButton::clicked,
            this, &CircuitCanvas::deleteSelected);

    connect(validate, &QPushButton::clicked,
            this, &CircuitCanvas::validateCanvas);

    auto *interactiveView = static_cast<CircuitGraphicsView *>(view_);

    interactiveView->componentClicked = [this](ComponentItem *component)
    {
        if (wireMode_)
        {
            completeWireConnection(component);
            return true;
        }

        return false;
    };
}

void CircuitCanvas::addComponent()
{
    auto *component = new ComponentItem(nextType_);
    scene_->addItem(component);

    int count = 0;

    for (QGraphicsItem *item : scene_->items())
    {
        if (dynamic_cast<ComponentItem *>(item))
            ++count;
    }

    component->setPos(
        80 + ((count - 1) % 7) * 190,
        100 + ((count - 1) / 7) * 150);

    connect(component, &ComponentItem::inputToggled,
            this, &CircuitCanvas::simulateCircuit);

    status_->setText(nextType_ + " component added.");
    simulateCircuit();
}

void CircuitCanvas::startWireConnection()
{
    wireSource_ = nullptr;
    wireMode_ = true;
    status_->setText(
        "Click the source component first, then the destination component.");
}

void CircuitCanvas::cancelWireConnection()
{
    wireSource_ = nullptr;
    wireMode_ = false;
    status_->setText("Wire connection cancelled.");
}

void CircuitCanvas::completeWireConnection(ComponentItem *destination)
{
    if (!destination)
        return;

    QString destinationType = destination->componentType();

    if (!wireSource_)
    {
        if (destinationType == "OUTPUT" || destinationType == "LED")
        {
            status_->setText("Select an INPUT or gate as the source.");
            return;
        }

        wireSource_ = destination;
        status_->setText(
            "Source selected. Now click the destination component.");
        return;
    }

    if (destination == wireSource_)
    {
        status_->setText("Choose a different destination component.");
        return;
    }

    int pinCount = destination->inputPinCount();

    if (pinCount <= 0)
    {
        status_->setText("The destination has no input pins.");
        wireSource_ = nullptr;
        return;
    }

    int inputPin = -1;

    for (int pin = 0; pin < pinCount; ++pin)
    {
        bool occupied = false;

        for (QGraphicsItem *item : scene_->items())
        {
            auto *wire = dynamic_cast<WireItem *>(item);

            if (wire &&
                wire->destinationComponent() == destination &&
                wire->destinationInputPin() == pin)
            {
                occupied = true;
                break;
            }
        }

        if (!occupied)
        {
            inputPin = pin;
            break;
        }
    }

    if (inputPin == -1)
    {
        status_->setText("All input pins are already connected.");
        wireSource_ = nullptr;
        return;
    }

    auto *wire = new WireItem(wireSource_, destination, inputPin);
    scene_->addItem(wire);

    wireSource_->addWire(wire);
    destination->addWire(wire);

    wireSource_ = nullptr;
    wireMode_ = false;

    simulateCircuit();
    status_->setText("Wire connected. Live simulation updated.");
}

void CircuitCanvas::simulateCircuit()
{
    QList<ComponentItem *> components;
    QList<WireItem *> wires;

    for (QGraphicsItem *item : scene_->items())
    {
        if (auto *component = dynamic_cast<ComponentItem *>(item))
            components.append(component);
        else if (auto *wire = dynamic_cast<WireItem *>(item))
            wires.append(wire);
    }

    QSet<ComponentItem *> inputComponents;

    for (ComponentItem *component : components)
    {
        if (component->componentType() == "INPUT")
            inputComponents.insert(component);
        else
            component->setLogicValue(0);
    }

    for (WireItem *wire : wires)
        wire->setLogicValue(-1);

    QSet<ComponentItem *> evaluated = inputComponents;
    bool changed = true;
    int passes = 0;
    const int maxPasses = components.size() + 1;

    while (changed && passes < maxPasses)
    {
        changed = false;
        ++passes;

        for (ComponentItem *component : components)
        {
            const QString type = component->componentType();

            if (type == "INPUT")
                continue;

            if (type == "D FLIP-FLOP")
                continue;

            const int pinCount = component->inputPinCount();
            QList<int> values(pinCount, 0);
            QList<bool> connected(pinCount, false);
            bool ready = true;

            for (WireItem *wire : wires)
            {
                if (wire->destinationComponent() != component)
                    continue;

                int pin = wire->destinationInputPin();

                if (pin < 0 || pin >= pinCount)
                    continue;

                ComponentItem *source = wire->sourceComponent();

                if (!source || !evaluated.contains(source))
                {
                    ready = false;
                    continue;
                }

                values[pin] = source->logicValue();
                connected[pin] = true;
            }

            if (!ready)
                continue;

            if (type != "OUTPUT" && type != "LED")
            {
                bool allPinsConnected = true;

                for (bool isConnected : connected)
                {
                    if (!isConnected)
                        allPinsConnected = false;
                }

                if (!allPinsConnected)
                    continue;
            }
            else if (!connected[0])
            {
                continue;
            }

            int result = 0;

            if (type == "OUTPUT" || type == "LED")
            {
                result = values[0];
            }
            else if (type == "NOT")
            {
                result = !values[0];
            }
            else if (type == "AND" || type == "NAND")
            {
                result = values[0] && values[1];

                if (type == "NAND")
                    result = !result;
            }
            else if (type == "OR" || type == "NOR")
            {
                result = values[0] || values[1];

                if (type == "NOR")
                    result = !result;
            }
            else if (type == "XOR" || type == "XNOR")
            {
                result = values[0] != values[1];

                if (type == "XNOR")
                    result = !result;
            }
            else
            {
                continue;
            }

            if (!evaluated.contains(component) ||
                component->logicValue() != result)
            {
                component->setLogicValue(result);
                evaluated.insert(component);
                changed = true;
            }
            else
            {
                evaluated.insert(component);
            }
        }
    }

    for (WireItem *wire : wires)
    {
        ComponentItem *source = wire->sourceComponent();

        if (source && evaluated.contains(source))
            wire->setLogicValue(source->logicValue());
        else
            wire->setLogicValue(-1);
    }

    bool hasUnevaluatedGate = false;

    for (ComponentItem *component : components)
    {
        QString type = component->componentType();

        if (type != "INPUT" &&
            type != "D FLIP-FLOP" &&
            !evaluated.contains(component))
        {
            hasUnevaluatedGate = true;
            break;
        }
    }

    if (hasUnevaluatedGate)
        status_->setText(
            "Live simulation: some components need connected inputs or may be in a feedback loop.");
    else
        status_->setText("Live simulation running.");
}

void CircuitCanvas::deleteSelected()
{
    QList<QGraphicsItem *> selectedItems = scene_->selectedItems();
    QList<ComponentItem *> componentsToDelete;
    QList<WireItem *> wiresToDelete;

    for (QGraphicsItem *item : selectedItems)
    {
        if (auto *component = dynamic_cast<ComponentItem *>(item))
            componentsToDelete.append(component);
        else if (auto *wire = dynamic_cast<WireItem *>(item))
            wiresToDelete.append(wire);
    }

    for (ComponentItem *component : componentsToDelete)
    {
        for (WireItem *wire : component->wires())
        {
            if (wire && !wiresToDelete.contains(wire))
                wiresToDelete.append(wire);
        }
    }

    for (WireItem *wire : wiresToDelete)
    {
        if (!wire)
            continue;

        if (wire->sourceComponent())
            wire->sourceComponent()->removeWire(wire);

        if (wire->destinationComponent())
            wire->destinationComponent()->removeWire(wire);

        delete wire;
    }

    for (ComponentItem *component : componentsToDelete)
        delete component;

    simulateCircuit();
    status_->setText("Selected components and connected wires deleted.");
}

void CircuitCanvas::validateCanvas()
{
    int inputs = 0;
    int outputs = 0;
    int components = 0;
    int wires = 0;

    for (QGraphicsItem *item : scene_->items())
    {
        if (auto *component = dynamic_cast<ComponentItem *>(item))
        {
            ++components;

            if (component->componentType() == "INPUT")
                ++inputs;

            if (component->componentType() == "OUTPUT" ||
                component->componentType() == "LED")
                ++outputs;
        }
        else if (dynamic_cast<WireItem *>(item))
        {
            ++wires;
        }
    }

    status_->setText(
        QString("Components: %1 | Inputs: %2 | Outputs/LEDs: %3 | Wires: %4")
            .arg(components)
            .arg(inputs)
            .arg(outputs)
            .arg(wires));
}