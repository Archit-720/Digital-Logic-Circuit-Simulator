#ifndef EXPRESSIONWIDGET_H
#define EXPRESSIONWIDGET_H

#include <QWidget>
#include <QString>

class QLineEdit;
class QLabel;
class QTextEdit;
class ExpressionCircuitDiagram;

class ExpressionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExpressionWidget(QWidget *parent = nullptr);

private slots:
    void evaluateExpression();
    void generateTruthTable();
    void simplifyExpression();
    void generateCircuit();

private:
    bool validateExpression(int &variableCount, QString &errorMessage);
    void showMessage(const QString &message);

    QLineEdit *expression_;
    QLineEdit *values_;
    QLabel *result_;
    QTextEdit *output_;
    ExpressionCircuitDiagram *circuitDiagram_;
};

#endif
