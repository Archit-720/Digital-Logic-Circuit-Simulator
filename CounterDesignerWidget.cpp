#include "CounterDesignerWidget.h"
#include "../cpp/sequential/CounterDesigner.h"

#include <QSpinBox>
#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QTextEdit>
#include <QScrollArea>
#include <QAbstractItemView>
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QFontMetrics>
#include <QScrollBar>
#include <QStringList>
#include <QFrame>
#include <QSizePolicy>
#include <algorithm>
#include <cmath>
#include <stdexcept>

static FlipFlopType counterFF(int index)
{
    switch (index) {
    case 0: return FlipFlopType::SR;
    case 1: return FlipFlopType::JK;
    case 2: return FlipFlopType::D;
    default: return FlipFlopType::T;
    }
}

class CounterCircuitDiagram : public QWidget
{
public:
    explicit CounterCircuitDiagram(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(250);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        setAttribute(Qt::WA_OpaquePaintEvent, false);
    }

    void setConfiguration(int bits, const QString &mode,
                          const QString &direction, const QString &flipFlop)
    {
        bits_ = bits;
        mode_ = mode;
        direction_ = direction;
        flipFlop_ = flipFlop;

        int spacing = bits_ <= 4 ? 145 : bits_ <= 7 ? 115 : 92;
        int requiredWidth = 90 + bits_ * spacing;

        setMinimumWidth(std::max(760, requiredWidth));
        setFixedHeight(270);
        updateGeometry();
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), palette().base());

        const int count = std::max(1, bits_);
        const int spacing = count <= 4 ? 145 : count <= 7 ? 115 : 92;
        const int blockWidth = count <= 7 ? 82 : 68;
        const int blockHeight = 72;
        const int startX = 42;
        const int blockY = 100;
        const int centerY = blockY + blockHeight / 2;

        painter.setPen(palette().text().color());
        QFont headingFont = painter.font();
        headingFont.setBold(true);
        headingFont.setPointSize(11);
        painter.setFont(headingFont);

        painter.drawText(
            QRect(12, 8, width() - 24, 24),
            Qt::AlignCenter,
            QString("%1-bit %2 %3 Counter Using %4 Flip-Flops")
                .arg(bits_)
                .arg(mode_)
                .arg(direction_)
                .arg(flipFlop_)
        );

        QFont normalFont = painter.font();
        normalFont.setBold(false);
        normalFont.setPointSize(9);
        painter.setFont(normalFont);

        painter.drawText(
            QRect(12, 34, width() - 24, 20),
            Qt::AlignCenter,
            mode_ == "Synchronous"
                ? "All flip-flops share a common clock"
                : "Clock signal ripples from one stage to the next"
        );

        const int clockY = 76;

        if (mode_ == "Synchronous") {
            painter.setPen(QPen(QColor(40, 110, 190), 2));
            painter.drawLine(
                startX + blockWidth / 2,
                clockY,
                startX + (count - 1) * spacing + blockWidth / 2,
                clockY
            );

            for (int i = 0; i < count; ++i) {
                int x = startX + i * spacing + blockWidth / 2;
                painter.drawLine(x, clockY, x, blockY);
                painter.drawEllipse(QPoint(x, clockY), 3, 3);
            }

            painter.setPen(palette().text().color());
            painter.drawText(
                QRect(10, 55, width() - 20, 18),
                Qt::AlignCenter,
                "CLK"
            );
        } else {
            painter.setPen(QPen(QColor(40, 110, 190), 2));
            int firstClockX = startX + blockWidth / 2;

            painter.drawText(
                QRect(firstClockX - 30, 56, 60, 18),
                Qt::AlignCenter,
                "CLK"
            );

            painter.drawLine(
                firstClockX,
                75,
                firstClockX,
                blockY
            );

            for (int i = 1; i < count; ++i) {
                int previousX = startX + (i - 1) * spacing;
                int currentX = startX + i * spacing;

                int previousOutputX = previousX + blockWidth;
                int currentClockX = currentX + blockWidth / 2;

                painter.drawLine(
                    previousOutputX,
                    centerY,
                    currentClockX,
                    centerY
                );

                painter.drawLine(
                    currentClockX,
                    centerY,
                    currentClockX,
                    blockY
                );

                QPolygon arrow;
                arrow << QPoint(currentClockX, centerY)
                      << QPoint(currentClockX - 6, centerY - 4)
                      << QPoint(currentClockX - 6, centerY + 4);

                painter.setBrush(QColor(40, 110, 190));
                painter.drawPolygon(arrow);
                painter.setBrush(Qt::NoBrush);
            }
        }

        for (int i = 0; i < count; ++i) {
            const int x = startX + i * spacing;

            QRect blockRect(x, blockY, blockWidth, blockHeight);

            painter.setPen(QPen(QColor(65, 95, 135), 2));
            painter.setBrush(QColor(225, 235, 248));
            painter.drawRoundedRect(blockRect, 7, 7);

            painter.setPen(QColor(25, 45, 75));

            QFont blockFont = painter.font();
            blockFont.setBold(true);
            blockFont.setPointSize(count <= 7 ? 10 : 8);
            painter.setFont(blockFont);

            painter.drawText(
                QRect(x + 3, blockY + 8, blockWidth - 6, 22),
                Qt::AlignCenter,
                QString("FF %1").arg(i)
            );

            QFont labelFont = painter.font();
            labelFont.setBold(false);
            labelFont.setPointSize(count <= 7 ? 9 : 8);
            painter.setFont(labelFont);

            painter.drawText(
                QRect(x + 3, blockY + 34, blockWidth - 6, 18),
                Qt::AlignCenter,
                flipFlop_ + " FF"
            );

            painter.setPen(QPen(QColor(40, 130, 85), 2));
            painter.drawLine(
                x + blockWidth,
                centerY,
                x + blockWidth + 13,
                centerY
            );

            painter.setPen(QColor(25, 100, 65));
            painter.drawText(
                QRect(x + blockWidth - 2, centerY + 7, 35, 18),
                Qt::AlignLeft,
                QString("Q%1").arg(i)
            );

            painter.setPen(QPen(QColor(65, 95, 135), 1));
            painter.setBrush(Qt::NoBrush);

            QPolygon clockTriangle;
            clockTriangle << QPoint(x + 5, blockY + blockHeight - 14)
                          << QPoint(x + 12, blockY + blockHeight - 10)
                          << QPoint(x + 5, blockY + blockHeight - 6);

            painter.drawPolyline(clockTriangle);
        }

        painter.setPen(palette().text().color());
        painter.setFont(normalFont);

        const int legendY = 205;

        painter.setPen(QPen(QColor(40, 110, 190), 2));
        painter.drawLine(24, legendY, 60, legendY);
        painter.setPen(palette().text().color());
        painter.drawText(
            QRect(66, legendY - 10, 130, 20),
            Qt::AlignLeft | Qt::AlignVCenter,
            mode_ == "Synchronous" ? "Common clock" : "Clock connection"
        );

        painter.setPen(QPen(QColor(40, 130, 85), 2));
        painter.drawLine(225, legendY, 261, legendY);
        painter.setPen(palette().text().color());
        painter.drawText(
            QRect(267, legendY - 10, 125, 20),
            Qt::AlignLeft | Qt::AlignVCenter,
            "Q output"
        );

        painter.drawText(
            QRect(12, 232, width() - 24, 24),
            Qt::AlignCenter,
            direction_ == "Up"
                ? "Count sequence: 0, 1, 2, ... , 2^n - 1, then repeat"
                : "Count sequence: 2^n - 1, ... , 2, 1, 0, then repeat"
        );
    }

private:
    int bits_ = 3;
    QString mode_ = "Synchronous";
    QString direction_ = "Up";
    QString flipFlop_ = "JK";
};

CounterDesignerWidget::CounterDesignerWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *page = new QWidget(scrollArea);
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(14, 14, 14, 14);
    root->setSpacing(10);

    auto *title = new QLabel("Counter Designer", page);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    root->addWidget(title);

    auto *description = new QLabel(
        "Generate counter state transitions, flip-flop excitation inputs, "
        "and a visual diagram of the selected counter configuration.",
        page
    );
    description->setWordWrap(true);
    root->addWidget(description);

    auto *settingsBox = new QGroupBox("Counter Configuration", page);
    auto *settings = new QHBoxLayout(settingsBox);
    settings->setSpacing(8);

    bits_ = new QSpinBox(settingsBox);
    bits_->setRange(1, 12);
    bits_->setValue(3);

    mode_ = new QComboBox(settingsBox);
    mode_->addItems({"Synchronous", "Asynchronous"});

    direction_ = new QComboBox(settingsBox);
    direction_->addItems({"Up", "Down"});

    flipFlop_ = new QComboBox(settingsBox);
    flipFlop_->addItems({"SR", "JK", "D", "T"});
    flipFlop_->setCurrentIndex(1);

    auto *generate = new QPushButton("Generate Design", settingsBox);

    settings->addWidget(new QLabel("Flip-flop bits:", settingsBox));
    settings->addWidget(bits_);
    settings->addWidget(new QLabel("Clock mode:", settingsBox));
    settings->addWidget(mode_);
    settings->addWidget(new QLabel("Direction:", settingsBox));
    settings->addWidget(direction_);
    settings->addWidget(new QLabel("Flip-flop:", settingsBox));
    settings->addWidget(flipFlop_);
    settings->addWidget(generate);

    root->addWidget(settingsBox);

    result_ = new QLabel(page);
    result_->setWordWrap(true);
    result_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    root->addWidget(result_);

    auto *diagramTitle = new QLabel("Counter Circuit Diagram", page);
    QFont sectionFont = diagramTitle->font();
    sectionFont.setBold(true);
    diagramTitle->setFont(sectionFont);
    root->addWidget(diagramTitle);

    diagram_ = new CounterCircuitDiagram(page);
    root->addWidget(diagram_);

    auto *diagramNote = new QLabel(
        "Diagram guide: blue lines show clock connections and green lines "
        "show Q outputs. Synchronous counters use a shared clock; "
        "asynchronous counters use ripple clock connections. This is a "
        "configuration diagram, not a synthesized gate-level circuit.",
        page
    );
    diagramNote->setWordWrap(true);
    root->addWidget(diagramNote);

    auto *tableTitle = new QLabel(
        "State Transition and Excitation Table",
        page
    );
    tableTitle->setFont(sectionFont);
    root->addWidget(tableTitle);

    table_ = new QTableWidget(page);
    table_->setAlternatingRowColors(true);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    table_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents
    );
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    root->addWidget(table_);

    auto *detailsTitle = new QLabel("Design Explanation", page);
    detailsTitle->setFont(sectionFont);
    root->addWidget(detailsTitle);

    details_ = new QTextEdit(page);
    details_->setReadOnly(true);
    details_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    details_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    details_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    root->addWidget(details_);

    root->addStretch(1);

    scrollArea->setWidget(page);
    outerLayout->addWidget(scrollArea);

    connect(
        generate,
        &QPushButton::clicked,
        this,
        &CounterDesignerWidget::designCounter
    );

    connect(
        bits_,
        &QSpinBox::valueChanged,
        this,
        [this]() { designCounter(); }
    );

    connect(
        mode_,
        &QComboBox::currentIndexChanged,
        this,
        [this]() { designCounter(); }
    );

    connect(
        direction_,
        &QComboBox::currentIndexChanged,
        this,
        [this]() { designCounter(); }
    );

    connect(
        flipFlop_,
        &QComboBox::currentIndexChanged,
        this,
        [this]() { designCounter(); }
    );

    designCounter();
}

void CounterDesignerWidget::designCounter()
{
    try {
        CounterMode mode = mode_->currentIndex() == 0
            ? CounterMode::Synchronous
            : CounterMode::Asynchronous;

        CounterDirection direction = direction_->currentIndex() == 0
            ? CounterDirection::Up
            : CounterDirection::Down;

        FlipFlopType ff = counterFF(flipFlop_->currentIndex());

        CounterDesign design = CounterDesigner::design(
            bits_->value(),
            mode,
            direction,
            ff
        );

        QStringList headers = {
            "Current (dec)",
            "Current (bin)",
            "Next (dec)",
            "Next (bin)"
        };

        for (int bit = 0; bit < design.bitCount; ++bit) {
            headers << QString("Q%1 Input")
                           .arg(design.bitCount - 1 - bit);
        }

        table_->clear();
        table_->setColumnCount(headers.size());
        table_->setRowCount(
            static_cast<int>(design.states.size())
        );
        table_->setHorizontalHeaderLabels(headers);

        QStringList sequence;

        for (int row = 0;
             row < static_cast<int>(design.states.size());
             ++row) {
            const CounterState &state = design.states[row];

            auto addCell = [this, row](
                int column,
                const QString &text
            ) {
                auto *item = new QTableWidgetItem(text);
                item->setTextAlignment(Qt::AlignCenter);
                table_->setItem(row, column, item);
            };

            addCell(0, QString::number(state.decimalValue));
            addCell(
                1,
                QString::fromStdString(state.binaryValue)
            );
            addCell(2, QString::number(state.nextDecimalValue));
            addCell(
                3,
                QString::fromStdString(state.nextBinaryValue)
            );

            for (int bit = 0;
                 bit < static_cast<int>(state.excitationInputs.size());
                 ++bit) {
                addCell(
                    4 + bit,
                    QString::fromStdString(
                        state.excitationInputs[bit]
                    )
                );
            }

            if (row < 16) {
                sequence << QString::fromStdString(
                    state.binaryValue
                );
            }
        }

        table_->resizeColumnsToContents();

        int tableHeight = table_->horizontalHeader()->height()
            + table_->frameWidth() * 2;

        for (int row = 0; row < table_->rowCount(); ++row) {
            tableHeight += table_->rowHeight(row);
        }

        table_->setMinimumHeight(tableHeight);
        table_->setMaximumHeight(tableHeight);

        const QString directionText =
            direction == CounterDirection::Up ? "Up" : "Down";

        const QString modeText =
            mode == CounterMode::Synchronous
                ? "Synchronous"
                : "Asynchronous";

        const QString ffText = flipFlop_->currentText();

        result_->setText(
            QString("%1-bit %2 %3 counter | Flip-flop: %4 | "
                    "States: %5 | Flip-flops required: %1")
                .arg(design.bitCount)
                .arg(modeText)
                .arg(directionText)
                .arg(ffText)
                .arg(design.states.size())
        );

        diagram_->setConfiguration(
            design.bitCount,
            modeText,
            directionText,
            ffText
        );

        QString explanation;
        explanation += "COUNTER CONFIGURATION\n";
        explanation += "Bits: "
            + QString::number(design.bitCount) + "\n";
        explanation += "Possible states: "
            + QString::number(design.states.size()) + "\n";
        explanation += "Flip-flop type: " + ffText + "\n";
        explanation += "Clock mode: " + modeText + "\n";
        explanation += "Direction: " + directionText + "\n\n";

        explanation += "STATE SEQUENCE PREVIEW\n";
        explanation += sequence.join(" -> ");

        if (design.states.size() > 16) {
            explanation += " -> ...";
        }

        explanation += "\n\nDESIGN NOTES\n";

        for (const std::string &note : design.notes) {
            explanation += "- "
                + QString::fromStdString(note) + "\n";
        }

        explanation += "\nHOW TO READ THE TABLE\n";
        explanation +=
            "Each row represents one current state and its required "
            "next state. Each Q-bit Input column contains the excitation "
            "input or inputs for that flip-flop at that transition.\n\n";

        explanation += "EXCITATION INPUT KEY\n";

        if (ff == FlipFlopType::D) {
            explanation += "D = next-state bit.\n";
        } else if (ff == FlipFlopType::T) {
            explanation +=
                "T = 0 means hold the current bit; "
                "T = 1 means toggle it.\n";
        } else if (ff == FlipFlopType::JK) {
            explanation +=
                "J controls setting and K controls resetting. "
                "X means the input may be either 0 or 1 for "
                "that transition.\n";
        } else {
            explanation +=
                "S controls setting and R controls resetting. "
                "X means the input may be either 0 or 1 for "
                "that transition. The forbidden S=1,R=1 "
                "combination is avoided.\n";
        }

        explanation +=
            "\nCIRCUIT DIAGRAM\n"
            "The diagram represents the number and arrangement of "
            "flip-flops for the selected clock mode. It does not "
            "show minimized Boolean equations or all logic gates.\n\n";

        explanation +=
            "Note: the state table provides excitation requirements, "
            "not minimized Boolean equations or a complete gate-level circuit.";

        details_->setPlainText(explanation);

        details_->document()->adjustSize();
        int detailsHeight =
            details_->document()->size().height()
            + details_->contentsMargins().top()
            + details_->contentsMargins().bottom()
            + 16;

        details_->setMinimumHeight(detailsHeight);
        details_->setMaximumHeight(detailsHeight);

        table_->resizeColumnsToContents();

        int finalTableHeight = table_->horizontalHeader()->height()
            + table_->frameWidth() * 2;

        for (int row = 0; row < table_->rowCount(); ++row) {
            finalTableHeight += table_->rowHeight(row);
        }

        table_->setMinimumHeight(finalTableHeight);
        table_->setMaximumHeight(finalTableHeight);
    }
    catch (const std::exception &error) {
        result_->setText(
            "Could not generate counter design: "
            + QString::fromLocal8Bit(error.what())
        );

        table_->setRowCount(0);
        table_->setColumnCount(0);
        table_->setMinimumHeight(100);
        table_->setMaximumHeight(100);
        details_->clear();
        details_->setMinimumHeight(100);
        details_->setMaximumHeight(100);
    }
}