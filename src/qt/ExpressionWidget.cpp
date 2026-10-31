#include "ExpressionWidget.h"

#include "../cpp/circuit/TruthTable.h"
#include "../cpp/expression/ExpressionParser.h"
#include "../cpp/expression/BooleanSimplifier.h"

#include <QAbstractScrollArea>
#include <QFont>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStringList>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <stdexcept>
#include <vector>

namespace
{
    struct DiagramNode
    {
        QString label;
        std::vector<int> children;
        int depth = 0;
        qreal x = 0;
        qreal y = 0;
    };

    QString normalizedOperator(const std::string &token)
    {
        QString value = QString::fromStdString(token).toUpper();

        if (value == "&" || value == "*") return "AND";
        if (value == "|") return "OR";
        if (value == "+") return "OR";
        if (value == "!") return "NOT";
        if (value == "~") return "NOT";

        return value;
    }

    bool isUnaryOperator(const QString &token)
    {
        return token == "NOT";
    }

    bool isBinaryOperator(const QString &token)
    {
        return token == "AND" || token == "OR" ||
               token == "XOR" || token == "NAND" ||
               token == "NOR" || token == "XNOR";
    }
}

class ExpressionCircuitDiagram : public QWidget
{
public:
    explicit ExpressionCircuitDiagram(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumHeight(250);
        setMinimumWidth(520);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
        setAutoFillBackground(false);
    }

    void setExpression(const QString &expression)
    {
        expression_ = expression;
        buildDiagram();
        updateGeometry();
        update();
    }

    QSize sizeHint() const override
    {
        int height = qMax(250, static_cast<int>(nodes_.size()) * 24);
        return QSize(760, height);
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.fillRect(rect(), QColor("#111827"));

        if (nodes_.empty())
        {
            painter.setPen(QColor("#94a3b8"));
            painter.setFont(QFont("Segoe UI", 11));
            painter.drawText(
                rect(),
                Qt::AlignCenter,
                "Enter a valid Boolean expression and click Generate Circuit."
            );
            return;
        }

        painter.setPen(QColor("#cbd5e1"));
        painter.setFont(QFont("Segoe UI", 10, QFont::DemiBold));
        painter.drawText(
            QRect(16, 10, width() - 32, 24),
            Qt::AlignLeft | Qt::AlignVCenter,
            "Circuit for: " + expression_
        );

        const int top = 55;
        const int bottom = 32;
        const int usableHeight = qMax(100, height() - top - bottom);

        qreal maxDepth = 0;

        for (const DiagramNode &node : nodes_)
            maxDepth = qMax(maxDepth, static_cast<qreal>(node.depth));

        const qreal xStep = qMax(
            105.0,
            (width() - 150.0) / qMax(1.0, maxDepth + 1.0)
        );

        std::vector<int> leaves;

        for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
        {
            if (nodes_[i].children.empty())
                leaves.push_back(i);
        }

        const qreal leafStep = qMax(
            44.0,
            static_cast<qreal>(usableHeight) /
                qMax(1, static_cast<int>(leaves.size()))
        );

        qreal nextLeafY = top + leafStep / 2.0;

        for (int index : leaves)
        {
            nodes_[index].x = 48 + nodes_[index].depth * xStep;
            nodes_[index].y = nextLeafY;
            nextLeafY += leafStep;
        }

        std::vector<int> order(nodes_.size());

        for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
            order[i] = i;

        std::sort(
            order.begin(),
            order.end(),
            [this](int a, int b)
            {
                return nodes_[a].depth > nodes_[b].depth;
            }
        );

        for (int index : order)
        {
            DiagramNode &node = nodes_[index];

            if (node.children.empty())
                continue;

            qreal totalY = 0;

            for (int child : node.children)
                totalY += nodes_[child].y;

            node.x = 48 + node.depth * xStep;
            node.y = totalY / node.children.size();
        }

        const qreal gateWidth = 70;
        const qreal gateHeight = 34;

        painter.setPen(QPen(QColor("#64748b"), 2));

        for (const DiagramNode &node : nodes_)
        {
            for (int child : node.children)
            {
                const DiagramNode &source = nodes_[child];

                QPainterPath wire;
                wire.moveTo(source.x + gateWidth / 2, source.y);
                wire.lineTo(node.x - gateWidth / 2, node.y);

                painter.setPen(QPen(QColor("#38bdf8"), 2));
                painter.drawPath(wire);

                painter.setBrush(QColor("#38bdf8"));
                painter.setPen(Qt::NoPen);
                painter.drawEllipse(
                    QPointF(source.x + gateWidth / 2, source.y),
                    3,
                    3
                );
            }
        }

        for (int i = 0; i < static_cast<int>(nodes_.size()); ++i)
        {
            const DiagramNode &node = nodes_[i];

            QRectF box(
                node.x - gateWidth / 2,
                node.y - gateHeight / 2,
                gateWidth,
                gateHeight
            );

            const bool isVariable =
                node.children.empty() &&
                !isBinaryOperator(node.label) &&
                !isUnaryOperator(node.label);

            QColor fill = isVariable
                ? QColor("#164e63")
                : QColor("#312e81");

            QColor border = isVariable
                ? QColor("#22d3ee")
                : QColor("#818cf8");

            painter.setBrush(fill);
            painter.setPen(QPen(border, 1.5));
            painter.drawRoundedRect(box, 7, 7);

            painter.setPen(QColor("#f8fafc"));
            painter.setFont(QFont("Segoe UI", 9, QFont::Bold));
            painter.drawText(box, Qt::AlignCenter, node.label);
        }

        const DiagramNode &root = nodes_[rootIndex_];

        painter.setPen(QColor("#34d399"));
        painter.setFont(QFont("Segoe UI", 9, QFont::DemiBold));
        painter.drawText(
            QRectF(
                root.x + gateWidth / 2 + 8,
                root.y - 12,
                58,
                24
            ),
            Qt::AlignLeft | Qt::AlignVCenter,
            "OUTPUT"
        );

        painter.setPen(QColor("#64748b"));
        painter.setFont(QFont("Segoe UI", 8));
        painter.drawText(
            QRect(16, height() - 25, width() - 32, 18),
            Qt::AlignLeft | Qt::AlignVCenter,
            "Diagram generated from the expression's postfix representation."
        );
    }

private:
    QString expression_;
    std::vector<DiagramNode> nodes_;
    int rootIndex_ = -1;

    void buildDiagram()
    {
        nodes_.clear();
        rootIndex_ = -1;

        if (expression_.trimmed().isEmpty())
            return;

        try
        {
            ExpressionParser parser;
            std::vector<std::string> postfix =
                parser.toPostfix(expression_.toStdString());

            std::stack<int> nodeStack;

            for (const std::string &rawToken : postfix)
            {
                QString token = normalizedOperator(rawToken);

                if (isUnaryOperator(token))
                {
                    if (nodeStack.empty())
                        throw std::runtime_error("Invalid NOT operator.");

                    int child = nodeStack.top();
                    nodeStack.pop();

                    DiagramNode node;
                    node.label = "NOT";
                    node.children.push_back(child);
                    node.depth = nodes_[child].depth + 1;

                    nodes_.push_back(node);
                    nodeStack.push(static_cast<int>(nodes_.size()) - 1);
                }
                else if (isBinaryOperator(token))
                {
                    if (nodeStack.size() < 2)
                        throw std::runtime_error("Invalid binary operator.");

                    int right = nodeStack.top();
                    nodeStack.pop();

                    int left = nodeStack.top();
                    nodeStack.pop();

                    DiagramNode node;
                    node.label = token;
                    node.children.push_back(left);
                    node.children.push_back(right);
                    node.depth = qMax(
                        nodes_[left].depth,
                        nodes_[right].depth
                    ) + 1;

                    nodes_.push_back(node);
                    nodeStack.push(static_cast<int>(nodes_.size()) - 1);
                }
                else
                {
                    DiagramNode node;
                    node.label = QString::fromStdString(rawToken).toUpper();
                    node.depth = 0;

                    nodes_.push_back(node);
                    nodeStack.push(static_cast<int>(nodes_.size()) - 1);
                }
            }

            if (nodeStack.size() == 1)
                rootIndex_ = nodeStack.top();
            else
                nodes_.clear();
        }
        catch (...)
        {
            nodes_.clear();
            rootIndex_ = -1;
        }
    }
};

ExpressionWidget::ExpressionWidget(QWidget *parent)
    : QWidget(parent),
      expression_(nullptr),
      values_(nullptr),
      result_(nullptr),
      output_(nullptr),
      circuitDiagram_(nullptr)
{
    setWindowTitle("Expression Lab");

    setStyleSheet(R"(
        QWidget {
            background-color: #0b1120;
            color: #e5e7eb;
            font-family: "Segoe UI";
            font-size: 10pt;
        }

        QScrollArea {
            background-color: #0b1120;
            border: none;
        }

        QScrollArea > QWidget > QWidget {
            background-color: #0b1120;
        }

        QGroupBox {
            background-color: #111827;
            border: 1px solid #334155;
            border-radius: 10px;
            margin-top: 14px;
            padding: 14px 10px 10px 10px;
            font-weight: bold;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
            color: #93c5fd;
        }

        QLineEdit {
            background-color: #0f172a;
            color: #f8fafc;
            border: 1px solid #475569;
            border-radius: 6px;
            padding: 9px;
            selection-background-color: #2563eb;
        }

        QLineEdit:focus {
            border: 2px solid #3b82f6;
        }

        QTextEdit {
            background-color: #0f172a;
            color: #e2e8f0;
            border: 1px solid #334155;
            border-radius: 8px;
            padding: 10px;
            font-family: Consolas;
            font-size: 10pt;
            selection-background-color: #2563eb;
        }

        QPushButton {
            background-color: #1e293b;
            color: #e2e8f0;
            border: 1px solid #475569;
            border-radius: 6px;
            padding: 9px 12px;
            font-weight: bold;
        }

        QPushButton:hover {
            background-color: #334155;
            border-color: #64748b;
        }

        QPushButton:pressed {
            background-color: #0f172a;
        }

        QPushButton#primaryButton {
            background-color: #2563eb;
            color: #ffffff;
            border: 1px solid #3b82f6;
        }

        QPushButton#primaryButton:hover {
            background-color: #1d4ed8;
        }

        QPushButton#clearButton {
            background-color: #3f1d2e;
            color: #fecdd3;
            border: 1px solid #9f1239;
        }

        QPushButton#clearButton:hover {
            background-color: #881337;
        }

        QLabel#pageTitle {
            font-size: 20pt;
            font-weight: bold;
            color: #f8fafc;
        }

        QLabel#subtitle {
            color: #94a3b8;
            font-size: 10pt;
        }

        QLabel#status {
            background-color: #172554;
            color: #bfdbfe;
            border: 1px solid #1d4ed8;
            border-radius: 7px;
            padding: 10px;
        }

        QLabel#sectionHint {
            color: #94a3b8;
            font-size: 9pt;
            font-weight: normal;
        }
    )");

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->setSpacing(0);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto *page = new QWidget(scrollArea);
    page->setObjectName("expressionPage");
    page->setStyleSheet("#expressionPage { background-color: #0b1120; }");

    auto *root = new QVBoxLayout(page);
    root->setContentsMargins(20, 18, 20, 20);
    root->setSpacing(10);

    auto *title = new QLabel("Expression Lab", page);
    title->setObjectName("pageTitle");
    root->addWidget(title);

    auto *subtitle = new QLabel(
        "Evaluate Boolean expressions, generate truth tables, simplify logic functions, and view circuit diagrams.",
        page
    );
    subtitle->setObjectName("subtitle");
    subtitle->setWordWrap(true);
    root->addWidget(subtitle);

    auto *inputGroup = new QGroupBox("Expression Input", page);
    auto *form = new QFormLayout(inputGroup);
    form->setContentsMargins(12, 18, 12, 12);
    form->setSpacing(10);
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    expression_ = new QLineEdit("(A AND B) OR NOT C", inputGroup);
    expression_->setPlaceholderText("Example: (A AND B) OR NOT C");
    expression_->setClearButtonEnabled(true);
    expression_->setMinimumHeight(38);

    values_ = new QLineEdit("A=1,B=0,C=1", inputGroup);
    values_->setPlaceholderText("Example: A=1,B=0,C=1");
    values_->setClearButtonEnabled(true);
    values_->setMinimumHeight(38);

    form->addRow("Boolean expression:", expression_);
    form->addRow("Input assignments:", values_);
    root->addWidget(inputGroup);

    auto *operatorGroup = new QGroupBox("Insert Operator", page);
    auto *operatorLayout = new QHBoxLayout(operatorGroup);
    operatorLayout->setContentsMargins(12, 18, 12, 12);
    operatorLayout->setSpacing(8);

    const QStringList operators = {
        "AND", "OR", "NOT", "NAND", "NOR", "XOR", "XNOR", "(", ")"
    };

    for (const QString &op : operators)
    {
        auto *button = new QPushButton(op, operatorGroup);
        button->setMinimumHeight(36);
        button->setCursor(Qt::PointingHandCursor);
        button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        connect(button, &QPushButton::clicked, this, [this, op]()
        {
            expression_->setFocus();

            QString insertion;

            if (op == "(" || op == ")")
                insertion = op;
            else if (op == "NOT")
                insertion = "NOT ";
            else
                insertion = " " + op + " ";

            expression_->insert(insertion);
        });

        operatorLayout->addWidget(button);
    }

    operatorLayout->addStretch();
    root->addWidget(operatorGroup);

    auto *buttonGroup = new QGroupBox("Expression Operations", page);
    auto *buttonLayout = new QHBoxLayout(buttonGroup);
    buttonLayout->setContentsMargins(12, 18, 12, 12);
    buttonLayout->setSpacing(8);

    auto *evaluateButton = new QPushButton("Evaluate", buttonGroup);
    evaluateButton->setObjectName("primaryButton");

    auto *truthTableButton = new QPushButton("Truth Table", buttonGroup);
    auto *simplifyButton = new QPushButton("Simplify", buttonGroup);
    auto *circuitButton = new QPushButton("Generate Circuit", buttonGroup);

    auto *clearButton = new QPushButton("Clear", buttonGroup);
    clearButton->setObjectName("clearButton");

    const QList<QPushButton *> operationButtons = {
        evaluateButton,
        truthTableButton,
        simplifyButton,
        circuitButton,
        clearButton
    };

    for (QPushButton *button : operationButtons)
    {
        button->setMinimumHeight(38);
        button->setCursor(Qt::PointingHandCursor);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        buttonLayout->addWidget(button);
    }

    root->addWidget(buttonGroup);

    result_ = new QLabel(
        "Ready. Enter an expression and select an operation.",
        page
    );
    result_->setObjectName("status");
    result_->setWordWrap(true);
    root->addWidget(result_);

    auto *outputGroup = new QGroupBox("Results", page);
    auto *outputLayout = new QVBoxLayout(outputGroup);
    outputLayout->setContentsMargins(12, 18, 12, 12);
    outputLayout->setSpacing(8);

    auto *outputHint = new QLabel(
        "Evaluation results, truth tables, and simplified expressions appear here.",
        outputGroup
    );
    outputHint->setObjectName("sectionHint");
    outputHint->setWordWrap(true);
    outputLayout->addWidget(outputHint);

    output_ = new QTextEdit(outputGroup);
    output_->setReadOnly(true);
    output_->setPlaceholderText("Your results will appear here.");
    output_->setMinimumHeight(220);
    output_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    outputLayout->addWidget(output_);
    root->addWidget(outputGroup);

    auto *diagramGroup = new QGroupBox("Circuit Diagram", page);
    auto *diagramLayout = new QVBoxLayout(diagramGroup);
    diagramLayout->setContentsMargins(12, 18, 12, 12);
    diagramLayout->setSpacing(8);

    auto *diagramHint = new QLabel(
        "Enter an expression and click Generate Circuit to display its logic diagram here.",
        diagramGroup
    );
    diagramHint->setObjectName("sectionHint");
    diagramHint->setWordWrap(true);
    diagramLayout->addWidget(diagramHint);

    circuitDiagram_ = new ExpressionCircuitDiagram(diagramGroup);
    circuitDiagram_->setExpression(QString());
    diagramLayout->addWidget(circuitDiagram_);

    root->addWidget(diagramGroup);
    root->addStretch();

    scrollArea->setWidget(page);
    outerLayout->addWidget(scrollArea);

    connect(evaluateButton, &QPushButton::clicked,
            this, &ExpressionWidget::evaluateExpression);

    connect(truthTableButton, &QPushButton::clicked,
            this, &ExpressionWidget::generateTruthTable);

    connect(simplifyButton, &QPushButton::clicked,
            this, &ExpressionWidget::simplifyExpression);

    connect(circuitButton, &QPushButton::clicked,
            this, &ExpressionWidget::generateCircuit);

    connect(clearButton, &QPushButton::clicked, this, [this]()
    {
        expression_->clear();
        values_->clear();
        output_->clear();
        circuitDiagram_->setExpression(QString());

        result_->setText("Cleared. Enter a new Boolean expression.");
        expression_->setFocus();
    });
}

bool ExpressionWidget::validateExpression(
    int &variableCount,
    QString &errorMessage
)
{
    try
    {
        ExpressionParser parser;

        std::vector<std::string> variables =
            parser.getVariables(expression_->text().toStdString());

        parser.toPostfix(expression_->text().toStdString());

        if (variables.empty())
        {
            errorMessage = "The expression must contain at least one variable.";
            return false;
        }

        std::set<char> letters;

        for (const std::string &variable : variables)
        {
            if (variable.size() != 1)
            {
                errorMessage = "Use single-letter variables A, B, C, and so on.";
                return false;
            }

            char letter = variable[0];

            if (letter >= 'a' && letter <= 'z')
                letter = static_cast<char>(letter - 'a' + 'A');

            if (letter < 'A' || letter > 'P')
            {
                errorMessage = "Use variables A through P for these tools.";
                return false;
            }

            letters.insert(letter);
        }

        variableCount = static_cast<int>(letters.size());

        for (int i = 0; i < variableCount; ++i)
        {
            char expected = static_cast<char>('A' + i);

            if (letters.find(expected) == letters.end())
            {
                errorMessage =
                    "Variables must be consecutive, starting with A. "
                    "For example, use A, B, C instead of A, C.";
                return false;
            }
        }

        return true;
    }
    catch (const std::exception &exception)
    {
        errorMessage = QString::fromStdString(exception.what());
        return false;
    }
}

void ExpressionWidget::showMessage(const QString &message)
{
    result_->setText(message);
}

void ExpressionWidget::evaluateExpression()
{
    try
    {
        int variableCount = 0;
        QString errorMessage;

        if (!validateExpression(variableCount, errorMessage))
        {
            showMessage("Invalid expression: " + errorMessage);
            output_->clear();
            return;
        }

        std::map<char, int> assignments;

        QStringList pairs = values_->text().split(',', Qt::SkipEmptyParts);

        for (const QString &pair : pairs)
        {
            QStringList parts = pair.split('=');

            if (parts.size() != 2)
                throw std::invalid_argument("Use assignments such as A=1,B=0,C=1.");

            QString name = parts[0].trimmed().toUpper();
            QString valueText = parts[1].trimmed();

            if (name.size() != 1 || name[0] < 'A' || name[0] > 'P')
                throw std::invalid_argument("Each variable must be a single letter from A to P.");

            bool ok = false;
            int value = valueText.toInt(&ok);

            if (!ok || (value != 0 && value != 1))
                throw std::invalid_argument("Each input value must be 0 or 1.");

            char variable = name[0].toLatin1();

            if (assignments.find(variable) != assignments.end())
                throw std::invalid_argument("A variable has been assigned more than once.");

            assignments[variable] = value;
        }

        std::vector<int> inputs(26, 0);

        for (int i = 0; i < variableCount; ++i)
        {
            char variable = static_cast<char>('A' + i);
            auto found = assignments.find(variable);

            if (found == assignments.end())
            {
                throw std::invalid_argument(
                    std::string("Missing input assignment for ") + variable + "."
                );
            }

            inputs[i] = found->second;
        }

        bool output = TruthTable::evaluate(
            expression_->text().toStdString(),
            inputs
        );

        showMessage(QString("Evaluation successful. Output = %1").arg(output ? 1 : 0));

        output_->setPlainText(
            "Expression: " + expression_->text() +
            "\nInput assignments: " + values_->text() +
            "\n\nOutput: " + QString::number(output ? 1 : 0)
        );
    }
    catch (const std::exception &exception)
    {
        showMessage("Evaluation failed.");
        output_->setPlainText(QString::fromStdString(exception.what()));
    }
}

void ExpressionWidget::generateTruthTable()
{
    try
    {
        int variableCount = 0;
        QString errorMessage;

        if (!validateExpression(variableCount, errorMessage))
        {
            showMessage("Invalid expression: " + errorMessage);
            output_->clear();
            return;
        }

        TruthTableResult table = TruthTable::generateDetailed(
            variableCount,
            expression_->text().toStdString()
        );

        QString text;

        for (int i = 0; i < variableCount; ++i)
            text += QString(QChar('A' + i)) + " | ";

        text += "Output\n";
        text += QString("-").repeated(variableCount * 4 + 7);
        text += "\n";

        for (const TruthTableRow &row : table.rows)
        {
            for (int value : row.inputs)
                text += QString::number(value) + " | ";

            text += QString::number(row.output) + "\n";
        }

        output_->setPlainText(text);
        showMessage(QString("Truth table generated: %1 rows.").arg(table.rows.size()));
    }
    catch (const std::exception &exception)
    {
        showMessage("Truth table generation failed.");
        output_->setPlainText(QString::fromStdString(exception.what()));
    }
}

void ExpressionWidget::simplifyExpression()
{
    try
    {
        int variableCount = 0;
        QString errorMessage;

        if (!validateExpression(variableCount, errorMessage))
        {
            showMessage("Invalid expression: " + errorMessage);
            output_->clear();
            return;
        }

        TruthTableResult table = TruthTable::generateDetailed(
            variableCount,
            expression_->text().toStdString()
        );

        std::vector<int> minterms;

        for (const TruthTableRow &row : table.rows)
        {
            if (row.output == 1)
            {
                int minterm = 0;

                for (int value : row.inputs)
                    minterm = (minterm << 1) | value;

                minterms.push_back(minterm);
            }
        }

        BooleanSimplificationResult simplified =
            BooleanSimplifier::simplifyDetailed(variableCount, minterms);

        QString text;

        text += "Original expression:\n";
        text += expression_->text() + "\n\n";

        text += "Function:\n";
        text += QString::fromStdString(simplified.originalFunction) + "\n\n";

        text += "Simplified expression:\n";
        text += QString::fromStdString(simplified.simplifiedExpression) + "\n\n";

        text += "Simplification steps:\n";

        for (const std::string &step : simplified.steps)
            text += QString::fromStdString(step) + "\n";

        output_->setPlainText(text);
        showMessage("Boolean simplification completed.");
    }
    catch (const std::exception &exception)
    {
        showMessage("Simplification failed.");
        output_->setPlainText(QString::fromStdString(exception.what()));
    }
}

void ExpressionWidget::generateCircuit()
{
    int variableCount = 0;
    QString errorMessage;

    if (!validateExpression(variableCount, errorMessage))
    {
        showMessage("Invalid expression: " + errorMessage);
        output_->clear();
        return;
    }

    try
    {
        circuitDiagram_->setExpression(expression_->text());
        circuitDiagram_->setMinimumHeight(
            qMax(250, 70 + variableCount * 75)
        );

        showMessage("Circuit diagram generated below the results.");

        output_->setPlainText(
            "Circuit generated successfully.\n\nExpression: " +
            expression_->text() +
            "\n\nThe circuit diagram is displayed directly below this results section."
        );
    }
    catch (const std::exception &exception)
    {
        showMessage("Circuit generation failed.");
        output_->setPlainText(QString::fromStdString(exception.what()));
    }
}
