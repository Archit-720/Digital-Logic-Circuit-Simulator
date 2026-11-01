#include "FlipFlopConversionWidget.h"
#include "../cpp/sequential/FlipFlopConverter.h"

#include <QComboBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QFont>
#include <QAbstractItemView>
#include <QStringList>
#include <QColor>
#include <QScrollArea>
#include <QPainter>
#include <QPainterPath>
#include <QFrame>
#include <QSizePolicy>
#include <QPolygon>
#include <QTextDocument>
#include <algorithm>

namespace {

FlipFlopType toType(int index)
{
    switch (index) {
    case 0: return FlipFlopType::SR;
    case 1: return FlipFlopType::JK;
    case 2: return FlipFlopType::D;
    default: return FlipFlopType::T;
    }
}

QString valueText(int value)
{
    return value < 0 ? "-" : QString::number(value);
}

}

class FlipFlopCircuitDiagram : public QWidget
{
public:
    explicit FlipFlopCircuitDiagram(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(280);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void setTypes(const QString &source, const QString &target)
    {
        source_ = source;
        target_ = target;
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.fillRect(rect(), palette().base());

        const QColor sourceColor("#dceafa");
        const QColor targetColor("#dff3e5");
        const QColor borderColor("#526b87");
        const QColor wireColor("#2878bd");
        const QColor outputColor("#278451");

        QFont headingFont = painter.font();
        headingFont.setBold(true);
        headingFont.setPointSize(12);
        painter.setFont(headingFont);
        painter.setPen(palette().text().color());

        painter.drawText(
            QRect(10, 8, width() - 20, 25),
            Qt::AlignCenter,
            "Flip-Flop Conversion Diagram"
        );

        QFont normalFont = painter.font();
        normalFont.setBold(false);
        normalFont.setPointSize(9);
        painter.setFont(normalFont);

        painter.drawText(
            QRect(10, 34, width() - 20, 22),
            Qt::AlignCenter,
            "The source next state is reproduced by selecting suitable target inputs."
        );

        const int boxWidth = 145;
        const int boxHeight = 105;
        const int boxY = 83;
        const int gap = 100;

        const int totalWidth = boxWidth * 2 + gap;
        const int sourceX = std::max(20, (width() - totalWidth) / 2);
        const int targetX = sourceX + boxWidth + gap;

        QRect sourceRect(sourceX, boxY, boxWidth, boxHeight);
        QRect targetRect(targetX, boxY, boxWidth, boxHeight);

        painter.setPen(QPen(borderColor, 2));
        painter.setBrush(sourceColor);
        painter.drawRoundedRect(sourceRect, 9, 9);

        painter.setBrush(targetColor);
        painter.drawRoundedRect(targetRect, 9, 9);

        QFont blockFont = painter.font();
        blockFont.setBold(true);
        blockFont.setPointSize(12);
        painter.setFont(blockFont);
        painter.setPen(QColor("#203650"));

        painter.drawText(
            QRect(sourceX + 5, boxY + 12, boxWidth - 10, 26),
            Qt::AlignCenter,
            source_ + " Flip-Flop"
        );

        painter.drawText(
            QRect(targetX + 5, boxY + 12, boxWidth - 10, 26),
            Qt::AlignCenter,
            target_ + " Flip-Flop"
        );

        QFont pinFont = painter.font();
        pinFont.setBold(false);
        pinFont.setPointSize(9);
        painter.setFont(pinFont);

        painter.drawText(
            QRect(sourceX + 8, boxY + 43, boxWidth - 16, 20),
            Qt::AlignCenter,
            "Source inputs"
        );

        painter.drawText(
            QRect(sourceX + 8, boxY + 65, boxWidth - 16, 20),
            Qt::AlignCenter,
            "Q(next)"
        );

        painter.drawText(
            QRect(targetX + 8, boxY + 43, boxWidth - 16, 20),
            Qt::AlignCenter,
            "Derived inputs"
        );

        painter.drawText(
            QRect(targetX + 8, boxY + 65, boxWidth - 16, 20),
            Qt::AlignCenter,
            "Same Q(next)"
        );

        const int sourceOutputX = sourceX + boxWidth;
        const int targetInputX = targetX;
        const int middleY = boxY + boxHeight / 2;

        painter.setPen(QPen(wireColor, 3));
        painter.drawLine(
            sourceOutputX,
            middleY,
            targetInputX,
            middleY
        );

        QPolygon arrow;
        arrow << QPoint(targetInputX, middleY)
              << QPoint(targetInputX - 9, middleY - 5)
              << QPoint(targetInputX - 9, middleY + 5);

        painter.setBrush(wireColor);
        painter.drawPolygon(arrow);
        painter.setBrush(Qt::NoBrush);

        painter.setPen(palette().text().color());
        painter.drawText(
            QRect(sourceOutputX + 5, middleY - 27,
                  gap - 10, 20),
            Qt::AlignCenter,
            "Q(next)"
        );

        painter.drawText(
            QRect(sourceX - 15, boxY + boxHeight + 16,
                  boxWidth + 30, 22),
            Qt::AlignCenter,
            "1. Calculate next state"
        );

        painter.drawText(
            QRect(targetX - 15, boxY + boxHeight + 16,
                  boxWidth + 30, 22),
            Qt::AlignCenter,
            "2. Derive target inputs"
        );

        const int noteY = boxY + boxHeight + 55;

        painter.setPen(QPen(outputColor, 2));
        painter.drawLine(
            std::max(20, (width() - 210) / 2),
            noteY,
            std::max(20, (width() - 210) / 2) + 30,
            noteY
        );

        painter.setPen(palette().text().color());
        painter.drawText(
            QRect(10, noteY + 10, width() - 20, 45),
            Qt::AlignCenter,
            "Verification goal: for each valid source state and input, "
            "the target produces the same next state."
        );

        painter.drawText(
            QRect(10, height() - 30, width() - 20, 20),
            Qt::AlignCenter,
            "Conceptual conversion diagram — not a synthesized gate-level circuit."
        );
    }

private:
    QString source_ = "SR";
    QString target_ = "D";
};

FlipFlopConversionWidget::FlipFlopConversionWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *page = new QWidget(scrollArea);
    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto *title = new QLabel("Flip-Flop Converter", page);
    QFont titleFont = title->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto *subtitle = new QLabel(
        "Convert one flip-flop type to another by deriving the target "
        "inputs from the source flip-flop's next state.",
        page
    );
    subtitle->setWordWrap(true);

    root->addWidget(title);
    root->addWidget(subtitle);

    auto *controls = new QGroupBox("Conversion Settings", page);
    auto *bar = new QHBoxLayout(controls);

    source_ = new QComboBox(controls);
    target_ = new QComboBox(controls);

    QStringList types = {"SR", "JK", "D", "T"};
    source_->addItems(types);
    target_->addItems(types);
    target_->setCurrentIndex(2);

    auto *convertButton = new QPushButton("Convert and Verify", controls);

    bar->addWidget(new QLabel("Source flip-flop:", controls));
    bar->addWidget(source_);
    bar->addSpacing(10);
    bar->addWidget(new QLabel("Target flip-flop:", controls));
    bar->addWidget(target_);
    bar->addSpacing(10);
    bar->addWidget(convertButton);

    root->addWidget(controls);

    status_ = new QLabel("Ready", page);
    status_->setWordWrap(true);
    status_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    root->addWidget(status_);

    auto *diagramBox = new QGroupBox("Conversion Circuit Diagram", page);
    auto *diagramLayout = new QVBoxLayout(diagramBox);

    diagram_ = new FlipFlopCircuitDiagram(diagramBox);
    diagramLayout->addWidget(diagram_);

    root->addWidget(diagramBox);

    auto *diagramNote = new QLabel(
        "The diagram updates with the selected source and target types. "
        "It shows the conversion process conceptually; it does not represent "
        "the individual gates needed to implement the conversion equations.",
        page
    );
    diagramNote->setWordWrap(true);
    root->addWidget(diagramNote);

    auto *tableBox = new QGroupBox("Conversion Truth Table", page);
    auto *tableLayout = new QVBoxLayout(tableBox);

    table_ = new QTableWidget(tableBox);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    table_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    table_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents
    );
    table_->horizontalHeader()->setStretchLastSection(true);

    tableLayout->addWidget(table_);
    root->addWidget(tableBox);

    auto *detailsBox = new QGroupBox("Equations and Derivation", page);
    auto *detailsLayout = new QVBoxLayout(detailsBox);

    result_ = new QTextEdit(detailsBox);
    result_->setReadOnly(true);
    result_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    result_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    result_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    detailsLayout->addWidget(result_);
    root->addWidget(detailsBox);

    root->addStretch(1);

    scrollArea->setWidget(page);
    outerLayout->addWidget(scrollArea);

    connect(
        convertButton,
        &QPushButton::clicked,
        this,
        &FlipFlopConversionWidget::convert
    );

    connect(
        source_,
        &QComboBox::currentIndexChanged,
        this,
        &FlipFlopConversionWidget::convert
    );

    connect(
        target_,
        &QComboBox::currentIndexChanged,
        this,
        &FlipFlopConversionWidget::convert
    );

    convert();
}

void FlipFlopConversionWidget::convert()
{
    const FlipFlopType source = toType(source_->currentIndex());
    const FlipFlopType target = toType(target_->currentIndex());

    const auto rows = FlipFlopConverter::conversionTable(source, target);

    table_->clear();
    table_->setRowCount(static_cast<int>(rows.size()));
    table_->setColumnCount(15);

    table_->setHorizontalHeaderLabels({
        "Q",
        "Src S", "Src R",
        "Src J", "Src K",
        "Src D", "Src T",
        "Q(next)",
        "Tgt S", "Tgt R",
        "Tgt J", "Tgt K",
        "Tgt D", "Tgt T",
        "Status"
    });

    int validRows = 0;
    int invalidRows = 0;

    for (int r = 0; r < static_cast<int>(rows.size()); ++r) {
        const auto &row = rows[r];

        const int values[] = {
            row.q,
            row.sourceS, row.sourceR,
            row.sourceJ, row.sourceK,
            row.sourceD, row.sourceT,
            row.nextQ,
            row.inputS, row.inputR,
            row.inputJ, row.inputK,
            row.inputD, row.inputT
        };

        for (int c = 0; c < 14; ++c) {
            auto *item = new QTableWidgetItem(valueText(values[c]));
            item->setTextAlignment(Qt::AlignCenter);

            if (!row.valid) {
                item->setBackground(QColor("#ffd6d6"));
            }

            table_->setItem(r, c, item);
        }

        QString statusText = row.valid
            ? "Valid"
            : "Invalid SR input";

        auto *statusItem = new QTableWidgetItem(statusText);
        statusItem->setTextAlignment(Qt::AlignCenter);

        if (!row.valid) {
            statusItem->setBackground(QColor("#ffd6d6"));
            ++invalidRows;
        } else {
            ++validRows;
        }

        table_->setItem(r, 14, statusItem);
    }

    table_->resizeColumnsToContents();

    int tableHeight = table_->horizontalHeader()->height()
        + table_->frameWidth() * 2;

    for (int row = 0; row < table_->rowCount(); ++row) {
        tableHeight += table_->rowHeight(row);
    }

    table_->setMinimumHeight(tableHeight);
    table_->setMaximumHeight(tableHeight);

    const QString sourceName =
        QString::fromStdString(
            FlipFlopConverter::typeName(source)
        );

    const QString targetName =
        QString::fromStdString(
            FlipFlopConverter::typeName(target)
        );

    diagram_->setTypes(sourceName, targetName);

    QString details;

    details += "1. SELECTED FLIP-FLOPS\n";
    details += "Source: " + sourceName + "\n";
    details += "Target: " + targetName + "\n\n";

    details += "2. SOURCE CHARACTERISTIC EQUATION\n";
    details += QString::fromStdString(
        FlipFlopConverter::characteristicEquation(source)
    );
    details += "\n\n";

    details += "3. TARGET EXCITATION TABLE\n";
    details += QString::fromStdString(
        FlipFlopConverter::excitationTableExplanation(target)
    );
    details += "\n\n";

    details += "4. TARGET INPUT EQUATIONS\n";
    details += QString::fromStdString(
        FlipFlopConverter::excitationEquation(target)
    );
    details += "\n\n";

    details += "5. CONVERSION METHOD\n";
    details +=
        "First, determine Q(next) using the selected source flip-flop "
        "and its inputs. Then derive the target inputs required to "
        "produce the same next state. A dash in the table indicates "
        "an input that is not used for that flip-flop type.\n\n";

    details += "6. TABLE SUMMARY\n";
    details += QString("Valid transitions: %1\n").arg(validRows);
    details += QString("Invalid source combinations: %1\n").arg(invalidRows);

    if (invalidRows > 0) {
        details +=
            "The invalid rows correspond to S = R = 1 for an active-high "
            "SR source flip-flop. They are excluded from the valid "
            "conversion behaviour.\n\n";
    }

    details +=
        "Verification note: the table derives target inputs from the "
        "source next state. For valid rows, the target input equations "
        "are selected to reproduce that next state. SR and JK excitation "
        "inputs may have don't-care alternatives; this implementation "
        "uses a simple valid choice rather than minimizing a circuit "
        "across all rows.\n\n";

    details += "7. CIRCUIT DIAGRAM\n";
    details +=
        "The diagram illustrates the conversion path from the source "
        "flip-flop's inputs and next state to the derived target inputs. "
        "It is a conceptual diagram, not a synthesized gate-level circuit.";

    result_->setPlainText(details);
    result_->document()->adjustSize();

    int detailsHeight =
        static_cast<int>(result_->document()->size().height())
        + result_->contentsMargins().top()
        + result_->contentsMargins().bottom()
        + 24;

    result_->setMinimumHeight(detailsHeight);
    result_->setMaximumHeight(detailsHeight);

    status_->setText(
        QString("%1 → %2 | %3 valid rows, %4 invalid rows")
            .arg(sourceName, targetName)
            .arg(validRows)
            .arg(invalidRows)
    );

    status_->setStyleSheet(
        invalidRows > 0
            ? "font-weight: bold; color: #a65a00;"
            : "font-weight: bold; color: #16803c;"
    );
}