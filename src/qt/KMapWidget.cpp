#include "KMapWidget.h"

#include "../cpp/kmap/KMapData.h"
#include "../cpp/kmap/KMapSolver.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QColor>
#include <QStringList>
#include <QFont>
#include <QAbstractItemView>
#include <QSizePolicy>
#include <QScrollArea>
#include <QWidget>

#include <vector>
#include <stdexcept>

namespace
{
    std::vector<int> readKNumbers(const QString &text)
    {
        std::vector<int> values;
        const QString input = text.trimmed();

        if (input.isEmpty())
            return values;

        for (const QString &part : input.split(','))
        {
            const QString trimmed = part.trimmed();

            bool ok = false;
            int number = trimmed.toInt(&ok);

            if (!ok || trimmed.isEmpty())
            {
                throw std::invalid_argument(
                    "Enter comma-separated integer indices.");
            }

            values.push_back(number);
        }

        return values;
    }

    int grayToBinary(int gray)
    {
        int binary = gray;

        while (gray >>= 1)
            binary ^= gray;

        return binary;
    }

    QString binaryText(int value, int bits)
    {
        QString text;

        for (int i = bits - 1; i >= 0; --i)
            text += ((value >> i) & 1) ? '1' : '0';

        return text.isEmpty() ? "-" : text;
    }

    QString variableNames(int first, int count)
    {
        QStringList names;

        for (int i = 0; i < count; ++i)
            names << QString(QChar('A' + first + i));

        return names.isEmpty() ? "-" : names.join("");
    }

    QString indexList(const std::vector<int> &values)
    {
        QStringList parts;

        for (int value : values)
            parts << QString::number(value);

        return parts.isEmpty() ? "none" : parts.join(", ");
    }
}

KMapWidget::KMapWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(0, 0);

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *page = new QWidget();
    page->setMinimumWidth(0);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(6);

    auto *title = new QLabel("Karnaugh Map Solver", page);

    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);

    auto *subtitle = new QLabel(
        "Build a Gray-code K-map, inspect groups, and verify the simplified function.",
        page);
    subtitle->setWordWrap(true);

    root->addWidget(title);
    root->addWidget(subtitle);

    auto *inputBox = new QGroupBox("Function Inputs", page);
    auto *form = new QFormLayout(inputBox);
    form->setContentsMargins(8, 8, 8, 8);
    form->setSpacing(5);

    variables_ = new QSpinBox(inputBox);
    variables_->setRange(1, 8);
    variables_->setValue(3);

    minterms_ = new QLineEdit("1,3,5,7", inputBox);
    minterms_->setPlaceholderText("Example: 1, 3, 5, 7");

    dontCares_ = new QLineEdit("", inputBox);
    dontCares_->setPlaceholderText("Optional: 0, 2");

    form->addRow("Number of variables:", variables_);
    form->addRow("Minterms:", minterms_);
    form->addRow("Don't-care indices:", dontCares_);

    root->addWidget(inputBox);

    auto *buttonRow = new QHBoxLayout();

    auto *solveButton = new QPushButton("Build K-map and Solve", page);
    auto *clearButton = new QPushButton("Reset Inputs", page);

    buttonRow->addWidget(solveButton);
    buttonRow->addWidget(clearButton);
    buttonRow->addStretch();

    root->addLayout(buttonRow);

    status_ = new QLabel("Ready", page);
    status_->setWordWrap(true);
    root->addWidget(status_);

    // K-Map section: the table itself does not have a restricted
    // vertical viewport. Its complete contents belong to the main page.
    auto *mapBox = new QGroupBox("K-map (Gray-code Order)", page);
    mapBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *mapLayout = new QVBoxLayout(mapBox);
    mapLayout->setContentsMargins(6, 6, 6, 6);
    mapLayout->setSpacing(4);

    table_ = new QTableWidget(mapBox);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setSelectionBehavior(QAbstractItemView::SelectItems);
    table_->setAlternatingRowColors(false);
    table_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    table_->setMinimumSize(0, 0);
    table_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    table_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    table_->verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);

    table_->horizontalHeader()->setMinimumSectionSize(35);
    table_->verticalHeader()->setMinimumSectionSize(25);
    table_->verticalHeader()->setDefaultSectionSize(28);
    table_->setCornerButtonEnabled(false);
    table_->setToolTip("Each cell tooltip shows its decimal minterm index.");

    mapLayout->addWidget(table_);
    root->addWidget(mapBox);

    // Solution section: it also expands naturally within the page.
    auto *solutionBox = new QGroupBox("Solution and Verification", page);
    solutionBox->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *solutionLayout = new QVBoxLayout(solutionBox);
    solutionLayout->setContentsMargins(6, 6, 6, 6);
    solutionLayout->setSpacing(4);

    solution_ = new QTextEdit(solutionBox);
    solution_->setReadOnly(true);
    solution_->setMinimumSize(0, 180);
    solution_->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Preferred);
    solution_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    solution_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    solutionLayout->addWidget(solution_);
    root->addWidget(solutionBox);

    root->addStretch(1);

    scrollArea->setWidget(page);
    outerLayout->addWidget(scrollArea);

    connect(
        solveButton,
        &QPushButton::clicked,
        this,
        &KMapWidget::solveMap);

    connect(clearButton, &QPushButton::clicked, this, [this]()
            {
                variables_->setValue(3);
                minterms_->setText("1,3,5,7");
                dontCares_->clear();
                solveMap();
            });

    solveMap();
}

void KMapWidget::solveMap()
{
    try
    {
        int n = variables_->value();

        KMapData data(
            n,
            readKNumbers(minterms_->text()),
            readKNumbers(dontCares_->text()));

        KMapResult result = KMapSolver::solve(data);

        updateMap(data, result);

        status_->setText(
            result.verified
                ? "Verification passed: the simplified expression matches the specified function."
                : "Verification failed: review the input or expression.");

        status_->setStyleSheet(
            result.verified
                ? "font-weight: bold; color: #16803c;"
                : "font-weight: bold; color: #c0392b;");
    }
    catch (const std::exception &error)
    {
        status_->setText("Could not build the K-map.");
        status_->setStyleSheet("font-weight: bold; color: #c0392b;");

        table_->clear();
        table_->setRowCount(0);
        table_->setColumnCount(0);

        solution_->setPlainText(
            QString("Input error: %1\n\n"
                    "Use comma-separated integer indices between 0 and "
                    "(2^number of variables - 1).\n"
                    "An index cannot be both a minterm and a don't-care.")
                .arg(QString::fromUtf8(error.what())));
    }
    catch (...)
    {
        status_->setText("An unknown error occurred.");
        status_->setStyleSheet("font-weight: bold; color: #c0392b;");

        table_->clear();
        table_->setRowCount(0);
        table_->setColumnCount(0);

        solution_->setPlainText("Check the inputs and try again.");
    }
}

void KMapWidget::updateMap(
    const KMapData &data,
    const KMapResult &result)
{
    const int n = data.variableCount();

    const int rowBits = n / 2;
    const int columnBits = n - rowBits;

    const int rowCount = 1 << rowBits;
    const int columnCount = 1 << columnBits;
    const int limit = 1 << n;

    table_->clear();
    table_->setRowCount(rowCount);
    table_->setColumnCount(columnCount);

    QStringList columnHeaders;
    QStringList rowHeaders;

    for (int column = 0; column < columnCount; ++column)
    {
        int gray = column ^ (column >> 1);
        columnHeaders << binaryText(gray, columnBits);
    }

    for (int row = 0; row < rowCount; ++row)
    {
        int gray = row ^ (row >> 1);
        rowHeaders << binaryText(gray, rowBits);
    }

    table_->setHorizontalHeaderLabels(columnHeaders);
    table_->setVerticalHeaderLabels(rowHeaders);

    const std::vector<QColor> groupColors = {
        QColor("#cce8ff"),
        QColor("#d5f5d5"),
        QColor("#ffe5b4"),
        QColor("#eadcff"),
        QColor("#ffd6df"),
        QColor("#d0f0ed"),
        QColor("#fff3a6"),
        QColor("#e0e0e0")};

    std::vector<int> groupForCell(limit, -1);

    for (size_t g = 0; g < result.groups.size(); ++g)
    {
        for (int cell : result.groups[g].cells)
        {
            if (cell >= 0 && cell < limit && groupForCell[cell] == -1)
                groupForCell[cell] = static_cast<int>(g);
        }
    }

    for (int row = 0; row < rowCount; ++row)
    {
        int rowGray = row ^ (row >> 1);
        int rowValue = grayToBinary(rowGray);

        for (int column = 0; column < columnCount; ++column)
        {
            int columnGray = column ^ (column >> 1);
            int columnValue = grayToBinary(columnGray);

            int index = (rowValue << columnBits) | columnValue;
            int value = data.cellValue(index);

            QString cellText = value == 1
                                   ? "1"
                               : value == -1 ? "X"
                                             : "0";

            auto *item = new QTableWidgetItem(cellText);
            item->setTextAlignment(Qt::AlignCenter);

            item->setToolTip(
                QString("Minterm index: %1\nBinary: %2\nCell value: %3")
                    .arg(index)
                    .arg(binaryText(index, n))
                    .arg(value == -1 ? "Don't-care" : QString::number(value)));

            int groupIndex = groupForCell[index];

            if (groupIndex >= 0)
            {
                item->setBackground(
                    groupColors[groupIndex % groupColors.size()]);

                item->setToolTip(
                    item->toolTip() +
                    QString("\nCovered by displayed group %1.")
                        .arg(groupIndex + 1));
            }

            QFont font = item->font();
            font.setPointSize(10);
            font.setBold(true);
            item->setFont(font);

            table_->setItem(row, column, item);
        }
    }

    table_->resizeColumnsToContents();
    table_->resizeRowsToContents();


    int tableHeight = table_->horizontalHeader()->sizeHint().height()
                    + table_->frameWidth() * 2;

    for (int row = 0; row < table_->rowCount(); ++row)
        tableHeight += table_->rowHeight(row);

    table_->setMinimumHeight(tableHeight);
    table_->setMaximumHeight(tableHeight);

    QString text;
    text += "INPUT FUNCTION\n";
    text += "Variables: " + variableNames(0, n) + "\n";
    text += "Minterms: " + indexList(data.minterms()) + "\n";
    text += "Don't-cares: " + indexList(data.dontCares()) + "\n\n";

    text += "SIMPLIFIED EXPRESSION\n";
    text += QString::fromStdString(result.expression) + "\n\n";

    text += "GROUP DETAILS\n";

    if (result.groups.empty())
    {
        text += "No product-term groups were extracted.\n";
    }
    else
    {
        for (size_t i = 0; i < result.groups.size(); ++i)
        {
            const auto &group = result.groups[i];

            text += QString("Group %1\n").arg(i + 1);
            text += "Term: " + QString::fromStdString(group.implicant) + "\n";
            text += "Covered cells: " + indexList(group.cells) + "\n";
            text += QString::fromStdString(group.explanation) + "\n\n";
        }
    }

    text += "DERIVATION\n";

    for (const std::string &step : result.steps)
        text += QString::fromStdString(step) + "\n\n";

    text += "VERIFICATION\n";
    text += result.verified
                ? "PASSED: all minterms evaluate to 1, and all zero-cells evaluate to 0.\n"
                : "FAILED: the simplified expression does not match the input function.\n";

    text += "\nNote: group colours identify the first group covering each cell. "
            "A cell can belong to more than one overlapping group, so check "
            "the group details as well.";

    solution_->setPlainText(text);

  
    solution_->document()->adjustSize();

    int solutionHeight = solution_->document()->size().height()
                       + solution_->document()->documentMargin() * 2
                       + solution_->frameWidth() * 2
                       + 20;

    solution_->setMinimumHeight(solutionHeight);
    solution_->setMaximumHeight(solutionHeight);
}
