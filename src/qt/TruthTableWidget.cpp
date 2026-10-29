#include "TruthTableWidget.h"
#include "../cpp/circuit/TruthTable.h"

#include <QAbstractItemView>
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>

#include <exception>
#include <stdexcept>

TruthTableWidget::TruthTableWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    auto *page = new QWidget(scrollArea);
    auto *root = new QVBoxLayout(page);

    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(10);

    auto *inputGroup = new QGroupBox("Expression Input", page);
    auto *form = new QFormLayout(inputGroup);

    expression_ = new QLineEdit("(A & B) | !C", inputGroup);
    expression_->setPlaceholderText(
        "Example: (A AND B) OR NOT C"
    );

    variables_ = new QSpinBox(inputGroup);
    variables_->setRange(1, 8);
    variables_->setValue(3);
    variables_->setToolTip(
        "Select the number of input variables. "
        "Variables are named A, B, C, D, and so on."
    );

    form->addRow("Boolean expression:", expression_);
    form->addRow("Number of variables:", variables_);

    root->addWidget(inputGroup);

    auto *buttonRow = new QHBoxLayout();

    auto *button = new QPushButton(
        "Generate Truth Table",
        page
    );

    button->setMinimumHeight(36);

    buttonRow->addWidget(button);
    buttonRow->addStretch();

    root->addLayout(buttonRow);

    auto *summaryGroup = new QGroupBox(
        "Expression Analysis",
        page
    );

    auto *summaryLayout = new QVBoxLayout(summaryGroup);

    summary_ = new QLabel(
        "Enter a Boolean expression and generate its truth table.",
        summaryGroup
    );

    summary_->setWordWrap(true);
    summary_->setTextInteractionFlags(
        Qt::TextSelectableByMouse
    );

    summaryLayout->addWidget(summary_);
    root->addWidget(summaryGroup);

    auto *tableGroup = new QGroupBox(
        "Complete Truth Table",
        page
    );

    auto *tableLayout = new QVBoxLayout(tableGroup);

    table_ = new QTableWidget(tableGroup);
    table_->setMinimumHeight(280);
    table_->setEditTriggers(
        QAbstractItemView::NoEditTriggers
    );

    table_->setSelectionBehavior(
        QAbstractItemView::SelectRows
    );

    table_->setSelectionMode(
        QAbstractItemView::SingleSelection
    );

    table_->setAlternatingRowColors(true);
    table_->setWordWrap(false);
    table_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    table_->verticalHeader()->setVisible(false);
    table_->verticalHeader()->setDefaultSectionSize(28);
    table_->horizontalHeader()->setMinimumSectionSize(65);
    table_->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Interactive
    );

    tableLayout->addWidget(table_);
    root->addWidget(tableGroup);

    auto *stepsGroup = new QGroupBox(
        "How Each Output Is Calculated",
        page
    );

    auto *stepsLayout = new QVBoxLayout(stepsGroup);

    steps_ = new QTextEdit(stepsGroup);
    steps_->setReadOnly(true);
    steps_->setMinimumHeight(180);
    steps_->setSizePolicy(
        QSizePolicy::Expanding,
        QSizePolicy::Expanding
    );

    stepsLayout->addWidget(steps_);
    root->addWidget(stepsGroup);

    status_ = new QLabel(
        "Ready to generate a truth table.",
        page
    );

    status_->setWordWrap(true);
    status_->setTextInteractionFlags(
        Qt::TextSelectableByMouse
    );

    QFont statusFont = status_->font();
    statusFont.setBold(true);
    status_->setFont(statusFont);

    root->addWidget(status_);

    root->addStretch();

    scrollArea->setWidget(page);
    outerLayout->addWidget(scrollArea);

    connect(
        button,
        &QPushButton::clicked,
        this,
        &TruthTableWidget::generateTable
    );

    connect(
        expression_,
        &QLineEdit::returnPressed,
        this,
        &TruthTableWidget::generateTable
    );

    connect(
        variables_,
        &QSpinBox::valueChanged,
        this,
        [this]()
        {
            status_->setText(
                "Variable count changed. Generate the table again to update the results."
            );
        }
    );

    generateTable();
}

void TruthTableWidget::generateTable()
{
    try
    {
        const int variableCount = variables_->value();
        const QString expressionText =
            expression_->text().trimmed();

        if (expressionText.isEmpty())
        {
            throw std::invalid_argument(
                "Please enter a Boolean expression."
            );
        }

        const TruthTableResult result =
            TruthTable::generateDetailed(
                variableCount,
                expressionText.toStdString()
            );

        QStringList headers;

        for (int i = 0; i < variableCount; ++i)
        {
            headers << QString(QChar('A' + i));
        }

        for (
            const std::string& intermediate :
            result.intermediateExpressions
        )
        {
            QString name =
                QString::fromStdString(intermediate).trimmed();

            if (name.isEmpty())
                name = "Intermediate";

            headers << name;
        }

        headers << "Final Output";

        table_->clear();
        table_->setRowCount(
            static_cast<int>(result.rows.size())
        );

        table_->setColumnCount(headers.size());
        table_->setHorizontalHeaderLabels(headers);

        for (
            int rowIndex = 0;
            rowIndex < static_cast<int>(result.rows.size());
            ++rowIndex
        )
        {
            const TruthTableRow& row =
                result.rows[rowIndex];

            for (int col = 0; col < variableCount; ++col)
            {
                auto *item = new QTableWidgetItem(
                    QString::number(row.inputs[col])
                );

                item->setTextAlignment(Qt::AlignCenter);

                table_->setItem(rowIndex, col, item);
            }

            for (
                int col = 0;
                col < static_cast<int>(
                    row.intermediateValues.size()
                );
                ++col
            )
            {
                const int tableColumn = variableCount + col;

                auto *item = new QTableWidgetItem(
                    QString::number(
                        row.intermediateValues[col]
                    )
                );

                item->setTextAlignment(Qt::AlignCenter);

                table_->setItem(
                    rowIndex,
                    tableColumn,
                    item
                );
            }

            auto *outputItem = new QTableWidgetItem(
                QString::number(row.output)
            );

            outputItem->setTextAlignment(Qt::AlignCenter);

            QFont outputFont = outputItem->font();
            outputFont.setBold(true);
            outputItem->setFont(outputFont);

            table_->setItem(
                rowIndex,
                headers.size() - 1,
                outputItem
            );
        }

        table_->resizeColumnsToContents();

        for (int col = 0; col < table_->columnCount(); ++col)
        {
            table_->setColumnWidth(
                col,
                qMax(75, table_->columnWidth(col))
            );
        }

        table_->horizontalHeader()->setStretchLastSection(true);

        QString analysis;

        analysis += "Expression: ";
        analysis += expressionText;
        analysis += "\nVariables: ";

        for (int i = 0; i < variableCount; ++i)
        {
            if (i > 0)
                analysis += ", ";

            analysis += QChar('A' + i);
        }

        analysis += "\nOperator precedence: NOT, AND, XOR, OR.";
        analysis += "\nTotal input combinations: ";
        analysis += QString::number(result.rows.size());
        analysis += "\nIntermediate operation columns: ";
        analysis += QString::number(
            result.intermediateExpressions.size()
        );

        summary_->setText(analysis);

        QString explanation;

        explanation += "EXPRESSION\n";
        explanation += expressionText;
        explanation += "\n\n";

        explanation += "SUPPORTED OPERATORS\n";
        explanation += "NOT: !, ~, NOT\n";
        explanation += "AND: &, *, AND\n";
        explanation += "OR: |, +, OR\n";
        explanation += "XOR: ^, XOR\n";
        explanation += "NAND: NAND\n";
        explanation += "NOR: NOR\n";
        explanation += "XNOR: XNOR\n";
        explanation += "Constants: 0 and 1\n\n";

        explanation += "HOW TO READ THE TABLE\n";
        explanation += "Each row represents one combination of input values. ";
        explanation += "Intermediate columns display the results of operations ";
        explanation += "evaluated by the expression parser. The Final Output ";
        explanation += "column gives the complete expression's result.\n\n";

        explanation += "ROW-BY-ROW RESULTS\n\n";

        for (
            int rowIndex = 0;
            rowIndex < static_cast<int>(result.rows.size());
            ++rowIndex
        )
        {
            const TruthTableRow& row =
                result.rows[rowIndex];

            explanation += "Row ";
            explanation += QString::number(rowIndex + 1);
            explanation += ": ";

            for (int i = 0; i < variableCount; ++i)
            {
                if (i > 0)
                    explanation += ", ";

                explanation += QChar('A' + i);
                explanation += "=";
                explanation += QString::number(row.inputs[i]);
            }

            explanation += " | Intermediate values: ";

            if (row.intermediateValues.empty())
            {
                explanation += "None";
            }
            else
            {
                for (
                    int i = 0;
                    i < static_cast<int>(
                        row.intermediateValues.size()
                    );
                    ++i
                )
                {
                    if (i > 0)
                        explanation += ", ";

                    explanation += QString::number(
                        row.intermediateValues[i]
                    );
                }
            }

            explanation += " | Final Output=";
            explanation += QString::number(row.output);
            explanation += "\n";
        }

        steps_->setPlainText(explanation);

        status_->setText(
            QString(
                "Success: generated %1 rows for %2 variable(s)."
            )
            .arg(result.rows.size())
            .arg(variableCount)
        );
    }
    catch (const std::exception& error)
    {
        table_->clear();
        table_->setRowCount(0);
        table_->setColumnCount(0);

        summary_->setText(
            "The expression could not be evaluated."
        );

        steps_->clear();

        status_->setText(
            QString("Error: %1").arg(
                QString::fromUtf8(error.what())
            )
        );
    }
    catch (...)
    {
        table_->clear();
        table_->setRowCount(0);
        table_->setColumnCount(0);

        summary_->setText(
            "The expression could not be evaluated."
        );

        steps_->clear();

        status_->setText(
            "Error: An unexpected problem occurred. Check the expression syntax."
        );
    }
}
