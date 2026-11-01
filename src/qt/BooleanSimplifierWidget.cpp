#include "BooleanSimplifierWidget.h"
#include "../cpp/expression/BooleanSimplifier.h"

#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QScrollArea>
#include <QWidget>
#include <QStringList>
#include <QTableWidgetItem>

#include <vector>
#include <set>
#include <stdexcept>
#include <string>

namespace
{
std::vector<int> parseNumbers(const QString& text)
{
    std::vector<int> values;

    if (text.trimmed().isEmpty())
        return values;

    for (const QString& part : text.split(',', Qt::SkipEmptyParts))
    {
        bool ok = false;
        int value = part.trimmed().toInt(&ok);

        if (!ok)
            throw std::invalid_argument(
                "Invalid number. Enter comma-separated integers."
            );

        values.push_back(value);
    }

    return values;
}

bool evaluateExpression(
    const std::string& expression,
    int variableCount,
    int row
)
{
    if (expression == "1")
        return true;

    if (expression == "0" || expression.empty())
        return false;

    size_t pos = 0;
    bool finalValue = false;

    while (pos < expression.size())
    {
        while (pos < expression.size() &&
               (expression[pos] == ' ' ||
                expression[pos] == '|' ||
                expression[pos] == '(' ||
                expression[pos] == ')'))
        {
            ++pos;
        }

        if (pos >= expression.size())
            break;

        bool termValue = true;

        while (pos < expression.size() &&
               expression[pos] != '|')
        {
            while (pos < expression.size() &&
                   (expression[pos] == ' ' ||
                    expression[pos] == '(' ||
                    expression[pos] == ')'))
            {
                ++pos;
            }

            if (pos >= expression.size() ||
                expression[pos] == '|')
            {
                break;
            }

            bool negated = false;

            if (expression[pos] == '!')
            {
                negated = true;
                ++pos;
            }

            if (pos >= expression.size())
                return false;

            char variable = expression[pos++];

            int index = variable - 'A';

            if (index < 0 || index >= variableCount)
                return false;

            bool bit =
                ((row >> (variableCount - 1 - index)) & 1) != 0;

            if (negated)
                bit = !bit;

            termValue = termValue && bit;
        }

        finalValue = finalValue || termValue;

        if (pos < expression.size() && expression[pos] == '|')
            ++pos;
    }

    return finalValue;
}
}

BooleanSimplifierWidget::BooleanSimplifierWidget(QWidget* parent)
    : QWidget(parent)
{
    auto* outerLayout = new QVBoxLayout(this);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget(scrollArea);
    auto* root = new QVBoxLayout(content);

    auto* inputGroup = new QGroupBox("Function Input", content);
    auto* form = new QFormLayout(inputGroup);

    variables_ = new QSpinBox(inputGroup);
    variables_->setRange(1, 8);
    variables_->setValue(3);

    minterms_ = new QLineEdit("1,3,5,7", inputGroup);
    minterms_->setPlaceholderText("Example: 1,3,5,7");

    dontCares_ = new QLineEdit("", inputGroup);
    dontCares_->setPlaceholderText("Optional, e.g. 0,2");

    form->addRow("Variable count:", variables_);
    form->addRow("Minterms (comma-separated):", minterms_);
    form->addRow("Don't-cares (optional):", dontCares_);

    root->addWidget(inputGroup);

    auto* simplifyButton = new QPushButton(
        "Simplify Function",
        content
    );

    root->addWidget(simplifyButton);

    auto* originalGroup = new QGroupBox(
        "Original Function",
        content
    );

    auto* originalLayout = new QVBoxLayout(originalGroup);

    original_ = new QLabel(originalGroup);
    original_->setWordWrap(true);
    original_->setTextInteractionFlags(Qt::TextSelectableByMouse);

    originalLayout->addWidget(original_);
    root->addWidget(originalGroup);

    auto* resultGroup = new QGroupBox(
        "Simplified Expression",
        content
    );

    auto* resultLayout = new QVBoxLayout(resultGroup);

    result_ = new QLabel(resultGroup);
    result_->setWordWrap(true);
    result_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    result_->setStyleSheet(
        "font-size: 18px; font-weight: bold; padding: 8px;"
    );

    resultLayout->addWidget(result_);
    root->addWidget(resultGroup);

    auto* stepsGroup = new QGroupBox(
        "Step-by-Step Solution",
        content
    );

    auto* stepsLayout = new QVBoxLayout(stepsGroup);

    steps_ = new QTextEdit(stepsGroup);
    steps_->setReadOnly(true);
    steps_->setMinimumHeight(220);
    steps_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Preferred
    );

    stepsLayout->addWidget(steps_);
    root->addWidget(stepsGroup);

    auto* tableGroup = new QGroupBox(
        "Truth Table and Verification",
        content
    );

    auto* tableLayout = new QVBoxLayout(tableGroup);

    truthTable_ = new QTableWidget(tableGroup);
    truthTable_->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );
    truthTable_->setSelectionMode(
        QAbstractItemView::NoSelection
    );
    truthTable_->setAlternatingRowColors(true);
    truthTable_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch
    );
    truthTable_->verticalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents
    );

    tableLayout->addWidget(truthTable_);

    verification_ = new QLabel(tableGroup);
    verification_->setWordWrap(true);
    verification_->setTextInteractionFlags(
        Qt::TextSelectableByMouse
    );

    tableLayout->addWidget(verification_);
    root->addWidget(tableGroup);

    root->addStretch();

    scrollArea->setWidget(content);
    outerLayout->addWidget(scrollArea);

    connect(
        simplifyButton,
        &QPushButton::clicked,
        this,
        &BooleanSimplifierWidget::simplify
    );

    connect(
        variables_,
        &QSpinBox::valueChanged,
        this,
        [this]()
        {
            simplify();
        }
    );

    simplify();
}

void BooleanSimplifierWidget::simplify()
{
    try
    {
        const int variableCount = variables_->value();

        const std::vector<int> minterms =
            parseNumbers(minterms_->text());

        const std::vector<int> dontCares =
            parseNumbers(dontCares_->text());

        const int limit = 1 << variableCount;

        std::set<int> required(
            minterms.begin(),
            minterms.end()
        );

        std::set<int> dontCareSet(
            dontCares.begin(),
            dontCares.end()
        );

        for (int value : required)
        {
            if (value < 0 || value >= limit)
            {
                throw std::invalid_argument(
                    "Minterms must be between 0 and " +
                    std::to_string(limit - 1) + "."
                );
            }
        }

        for (int value : dontCareSet)
        {
            if (value < 0 || value >= limit)
            {
                throw std::invalid_argument(
                    "Don't-care values must be between 0 and " +
                    std::to_string(limit - 1) + "."
                );
            }
        }

        for (int value : required)
            dontCareSet.erase(value);

        const std::vector<int> cleanedDontCares(
            dontCareSet.begin(),
            dontCareSet.end()
        );

        const BooleanSimplificationResult solution =
            BooleanSimplifier::simplifyDetailed(
                variableCount,
                std::vector<int>(
                    required.begin(),
                    required.end()
                ),
                cleanedDontCares
            );

        original_->setText(
            QString::fromStdString(solution.originalFunction)
        );

        result_->setText(
            "F = " +
            QString::fromStdString(solution.simplifiedExpression)
        );

        QString stepText;

        for (const std::string& step : solution.steps)
        {
            stepText += QString::fromStdString(step) + "\n";
        }

        steps_->setPlainText(stepText);

        truthTable_->clear();

        QStringList headers;

        for (int i = 0; i < variableCount; ++i)
        {
            headers << QString(QChar(
                static_cast<char>('A' + i)
            ));
        }

        headers << "Original F" << "Simplified F";

        truthTable_->setColumnCount(headers.size());
        truthTable_->setHorizontalHeaderLabels(headers);
        truthTable_->setRowCount(limit);

        bool equivalent = true;

        for (int row = 0; row < limit; ++row)
        {
            for (int col = 0; col < variableCount; ++col)
            {
                const int bit =
                    (row >> (variableCount - 1 - col)) & 1;

                auto* item = new QTableWidgetItem(
                    QString::number(bit)
                );

                item->setTextAlignment(Qt::AlignCenter);

                truthTable_->setItem(row, col, item);
            }

            const bool originalValue = required.count(row) != 0;

            const bool simplifiedValue = evaluateExpression(
                solution.simplifiedExpression,
                variableCount,
                row
            );

            auto* originalItem = new QTableWidgetItem(
                originalValue ? "1" : "0"
            );

            auto* simplifiedItem = new QTableWidgetItem(
                simplifiedValue ? "1" : "0"
            );

            originalItem->setTextAlignment(Qt::AlignCenter);
            simplifiedItem->setTextAlignment(Qt::AlignCenter);

            truthTable_->setItem(
                row,
                variableCount,
                originalItem
            );

            truthTable_->setItem(
                row,
                variableCount + 1,
                simplifiedItem
            );

            if (!dontCareSet.count(row) &&
                originalValue != simplifiedValue)
            {
                equivalent = false;
            }
        }

        if (equivalent)
        {
            verification_->setText(
                "Verification passed: the simplified expression "
                "matches the original function for every required "
                "input combination. Don't-care rows are excluded "
                "from the equivalence check."
            );

            verification_->setStyleSheet(
                "color: #16803c; font-weight: bold; padding: 6px;"
            );
        }
        else
        {
            verification_->setText(
                "Verification failed: the expressions differ on "
                "at least one required input combination."
            );

            verification_->setStyleSheet(
                "color: #c62828; font-weight: bold; padding: 6px;"
            );
        }

        truthTable_->resizeRowsToContents();
        truthTable_->setMinimumHeight(
            truthTable_->horizontalHeader()->height() +
            truthTable_->verticalHeader()->length() + 4
        );
    }
    catch (const std::exception& error)
    {
        QMessageBox::warning(
            this,
            "Invalid Input",
            QString::fromUtf8(error.what())
        );
    }
    catch (...)
    {
        QMessageBox::warning(
            this,
            "Invalid Input",
            "Please enter valid minterm and don't-care values."
        );
    }
}
