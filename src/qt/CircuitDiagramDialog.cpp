#include "CircuitDiagramDialog.h"

#include "../cpp/expression/ExpressionParser.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPathItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>
#include <QWheelEvent>
#include <QMessageBox>
#include <QBrush>
#include <QPen>
#include <QFont>

#include <vector>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace
{
    struct CircuitNode
    {
        QString label;
        QString type;
        CircuitNode *left = nullptr;
        CircuitNode *right = nullptr;
        int depth = 0;
        qreal y = 0;
        qreal x = 0;
    };

    class ZoomableGraphicsView : public QGraphicsView
    {
    public:
        explicit ZoomableGraphicsView(QGraphicsScene *scene, QWidget *parent = nullptr)
            : QGraphicsView(scene, parent)
        {
            setRenderHint(QPainter::Antialiasing);
            setRenderHint(QPainter::TextAntialiasing);
            setDragMode(QGraphicsView::ScrollHandDrag);
            setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
            setResizeAnchor(QGraphicsView::AnchorUnderMouse);
            setBackgroundBrush(QColor("#f5f8fc"));
        }

    protected:
        void wheelEvent(QWheelEvent *event) override
        {
            const qreal factor = event->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
            scale(factor, factor);
        }
    };

    QColor gateColor(const QString &type)
    {
        if (type == "AND" || type == "NAND")
            return QColor("#dbeafe");

        if (type == "OR" || type == "NOR")
            return QColor("#dcfce7");

        if (type == "NOT")
            return QColor("#fef3c7");

        if (type == "XOR" || type == "XNOR")
            return QColor("#f3e8ff");

        return QColor("#e2e8f0");
    }
}

CircuitDiagramDialog::CircuitDiagramDialog(
    const QString &expression,
    QWidget *parent)
    : QDialog(parent),
      scene_(new QGraphicsScene(this)),
      view_(new ZoomableGraphicsView(scene_, this))
{
    setWindowTitle("Generated Logic Circuit");
    resize(1150, 760);
    setMinimumSize(800, 550);

    setStyleSheet(R"(
        QDialog {
            background-color: #f5f8fc;
        }
        QPushButton {
            background-color: #2563eb;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #1d4ed8;
        }
    )");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->addWidget(view_, 1);

    auto *controls = new QHBoxLayout();

    auto *zoomIn = new QPushButton("Zoom In", this);
    auto *zoomOut = new QPushButton("Zoom Out", this);
    auto *fit = new QPushButton("Fit Circuit", this);
    auto *close = new QPushButton("Close", this);

    controls->addWidget(zoomIn);
    controls->addWidget(zoomOut);
    controls->addWidget(fit);
    controls->addStretch();
    controls->addWidget(close);

    layout->addLayout(controls);

    connect(zoomIn, &QPushButton::clicked, this, [this]()
            { view_->scale(1.2, 1.2); });

    connect(zoomOut, &QPushButton::clicked, this, [this]()
            { view_->scale(1.0 / 1.2, 1.0 / 1.2); });

    connect(fit, &QPushButton::clicked, this, [this]()
            { view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio); });

    connect(close, &QPushButton::clicked, this, &QDialog::accept);

    try
    {
        drawCircuit(expression);
    }
    catch (const std::exception &exception)
    {
        QMessageBox::critical(
            this,
            "Circuit Generation Error",
            QString::fromStdString(exception.what()));
    }
}

void CircuitDiagramDialog::drawCircuit(const QString &expression)
{
    scene_->clear();

    ExpressionParser parser;
    const auto postfix = parser.toPostfix(expression.toStdString());

    std::vector<std::unique_ptr<CircuitNode>> nodes;
    std::vector<CircuitNode *> stack;

    for (const std::string &tokenString : postfix)
    {
        QString token = QString::fromStdString(tokenString).toUpper();

        if (token == "NOT")
        {
            if (stack.empty())
                throw std::runtime_error("NOT gate is missing an input.");

            auto node = std::make_unique<CircuitNode>();
            node->type = "NOT";
            node->label = "NOT";
            node->left = stack.back();
            stack.pop_back();
            node->depth = node->left->depth + 1;

            CircuitNode *raw = node.get();
            nodes.push_back(std::move(node));
            stack.push_back(raw);
        }
        else if (token == "AND" || token == "OR" ||
                 token == "XOR" || token == "NAND" ||
                 token == "NOR" || token == "XNOR")
        {
            if (stack.size() < 2)
                throw std::runtime_error("A logic gate is missing an input.");

            auto node = std::make_unique<CircuitNode>();
            node->type = token;
            node->label = token;
            node->right = stack.back();
            stack.pop_back();
            node->left = stack.back();
            stack.pop_back();

            node->depth = std::max(
                              node->left->depth,
                              node->right->depth) +
                          1;

            CircuitNode *raw = node.get();
            nodes.push_back(std::move(node));
            stack.push_back(raw);
        }
        else
        {
            auto node = std::make_unique<CircuitNode>();
            node->type = "INPUT";
            node->label = QString::fromStdString(tokenString);

            CircuitNode *raw = node.get();
            nodes.push_back(std::move(node));
            stack.push_back(raw);
        }
    }

    if (stack.size() != 1)
        throw std::runtime_error("The expression could not form one circuit output.");

    CircuitNode *root = stack.back();

    int leafCount = 0;

    auto countLeaves = [&](auto &&self, CircuitNode *node) -> void
    {
        if (node->type == "INPUT")
        {
            node->y = 100 + leafCount * 110;
            ++leafCount;
            return;
        }

        self(self, node->left);

        if (node->right)
            self(self, node->right);

        if (node->right)
            node->y = (node->left->y + node->right->y) / 2.0;
        else
            node->y = node->left->y;
    };

    countLeaves(countLeaves, root);

    const qreal xStart = 100;
    const qreal xSpacing = 190;

    auto assignPositions = [&](auto &&self, CircuitNode *node) -> void
    {
        node->x = xStart + node->depth * xSpacing;

        if (node->left)
            self(self, node->left);

        if (node->right)
            self(self, node->right);
    };

    assignPositions(assignPositions, root);

    QPen wirePen(QColor("#334155"), 2.4);
    wirePen.setCapStyle(Qt::RoundCap);
    wirePen.setJoinStyle(Qt::RoundJoin);

    QFont labelFont("Segoe UI", 11, QFont::DemiBold);

    auto addLabel = [&](const QString &text, qreal x, qreal y,
                        const QColor &color = QColor("#172033"))
    {
        auto *item = scene_->addText(text, labelFont);
        item->setDefaultTextColor(color);
        item->setPos(x, y);
        return item;
    };

    auto addWire = [&](const QPainterPath &path)
    {
        auto *item = scene_->addPath(path, wirePen);
        item->setZValue(-1);
    };

    auto drawNode = [&](auto &&self, CircuitNode *node) -> void
    {
        const qreal inputX = node->x;
        const qreal centerY = node->y;

        if (node->type == "INPUT")
        {
            const qreal radius = 19;

            auto *circle = scene_->addEllipse(
                inputX - radius,
                centerY - radius,
                radius * 2,
                radius * 2,
                QPen(QColor("#2563eb"), 2),
                QBrush(QColor("#dbeafe")));

            circle->setZValue(1);

            addLabel(node->label,
                     inputX - 8,
                     centerY - 12,
                     QColor("#1d4ed8"));

            return;
        }

        self(self, node->left);

        if (node->right)
            self(self, node->right);

        const qreal gateLeft = node->x;
        const qreal gateRight = node->x + 90;
        const qreal gateTop = node->y - 36;
        const qreal gateBottom = node->y + 36;

        auto connectInput = [&](CircuitNode *source, qreal targetY)
        {
            QPainterPath path;
            const qreal startX =
                source->type == "INPUT" ? source->x + 19 : source->x + 90;

            path.moveTo(startX, source->y);

            const qreal middleX = (startX + gateLeft) / 2.0;

            path.lineTo(middleX, source->y);
            path.lineTo(middleX, targetY);
            path.lineTo(gateLeft, targetY);

            addWire(path);
        };

        if (node->type == "NOT")
        {
            connectInput(node->left, node->y);

            QPainterPath triangle;
            triangle.moveTo(gateLeft, gateTop);
            triangle.lineTo(gateLeft, gateBottom);
            triangle.lineTo(gateRight - 10, node->y);
            triangle.closeSubpath();

            scene_->addPath(
                triangle,
                QPen(QColor("#b45309"), 2),
                QBrush(gateColor(node->type)));

            scene_->addEllipse(
                gateRight - 7,
                node->y - 7,
                14,
                14,
                QPen(QColor("#b45309"), 2),
                QBrush(Qt::white));

            addLabel("NOT", gateLeft + 15, gateTop + 5);
        }
        else
        {
            const qreal firstY = node->left->y;
            const qreal secondY = node->right->y;

            connectInput(node->left, node->y - 15);
            connectInput(node->right, node->y + 15);

            if (node->type == "AND" || node->type == "NAND")
            {
                QPainterPath shape;
                shape.moveTo(gateLeft, gateTop);
                shape.lineTo(gateLeft + 40, gateTop);
                shape.cubicTo(
                    gateRight + 10, gateTop,
                    gateRight + 10, gateBottom,
                    gateLeft + 40, gateBottom);
                shape.lineTo(gateLeft, gateBottom);
                shape.closeSubpath();

                scene_->addPath(
                    shape,
                    QPen(QColor("#2563eb"), 2),
                    QBrush(gateColor(node->type)));
            }
            else
            {
                QPainterPath shape;
                shape.moveTo(gateLeft, gateTop);
                shape.cubicTo(
                    gateLeft + 28, gateTop + 4,
                    gateLeft + 48, gateTop + 4,
                    gateRight, node->y);
                shape.cubicTo(
                    gateLeft + 48, gateBottom - 4,
                    gateLeft + 28, gateBottom - 4,
                    gateLeft, gateBottom);
                shape.cubicTo(
                    gateLeft + 18, node->y + 10,
                    gateLeft + 18, node->y - 10,
                    gateLeft, gateTop);
                shape.closeSubpath();

                scene_->addPath(
                    shape,
                    QPen(QColor("#15803d"), 2),
                    QBrush(gateColor(node->type)));
            }

            if (node->type == "NAND" ||
                node->type == "NOR" ||
                node->type == "XNOR")
            {
                scene_->addEllipse(
                    gateRight - 6,
                    node->y - 6,
                    12,
                    12,
                    QPen(QColor("#334155"), 1.5),
                    QBrush(Qt::white));
            }

            addLabel(node->label, gateLeft + 12, gateTop + 5);

            Q_UNUSED(firstY);
            Q_UNUSED(secondY);
        }

        const qreal outputX =
            (node->type == "NOT") ? gateRight + 7 : gateRight;

        QPainterPath outputWire;
        outputWire.moveTo(outputX, node->y);
        outputWire.lineTo(node->x + 90, node->y);
        addWire(outputWire);
    };

    drawNode(drawNode, root);

    const qreal outputX = root->x + 115;

    QPainterPath finalWire;
    finalWire.moveTo(root->x + 90, root->y);
    finalWire.lineTo(outputX, root->y);
    addWire(finalWire);

    QPainterPath outputPath;
    outputPath.addRoundedRect(
        QRectF(outputX, root->y - 22, 105, 44),
        8,
        8);

    auto *outputBox = scene_->addPath(
        outputPath,
        QPen(QColor("#047857"), 2),
        QBrush(QColor("#d1fae5")));

    outputBox->setZValue(1);

    outputBox->setZValue(1);

    addLabel("OUTPUT", outputX + 12, root->y - 12, QColor("#065f46"));

    const qreal sceneWidth = outputX + 150;
    const qreal sceneHeight = std::max(300, leafCount * 110 + 100);

    scene_->setSceneRect(0, 0, sceneWidth, sceneHeight);

    view_->fitInView(scene_->sceneRect(), Qt::KeepAspectRatio);
}
