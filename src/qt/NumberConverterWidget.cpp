#include "NumberConverterWidget.h"

#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QFont>
#include <QStringList>
#include <QMessageBox>

#include <limits>

NumberConverterWidget::NumberConverterWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *root = new QVBoxLayout(this);

    auto *title = new QLabel("Number System Converter", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);
    root->addWidget(title);

    auto *description = new QLabel(
        "Convert numbers between binary, octal, decimal, and hexadecimal "
        "bases, with a step-by-step explanation.",
        this
    );
    description->setWordWrap(true);
    root->addWidget(description);

    auto *inputBox = new QGroupBox("Conversion Settings", this);
    auto *form = new QFormLayout(inputBox);

    number_ = new QLineEdit("101101", this);
    number_->setPlaceholderText("Enter a number, e.g. 101101");

    fromBase_ = new QComboBox(this);
    toBase_ = new QComboBox(this);

    for (int base = 2; base <= 16; ++base) {
        QString label = QString::number(base);

        if (base == 2) label += " (Binary)";
        else if (base == 8) label += " (Octal)";
        else if (base == 10) label += " (Decimal)";
        else if (base == 16) label += " (Hexadecimal)";

        fromBase_->addItem(label, base);
        toBase_->addItem(label, base);
    }

    fromBase_->setCurrentIndex(0);
    toBase_->setCurrentIndex(8);

    form->addRow("Input number:", number_);
    form->addRow("Source base:", fromBase_);
    form->addRow("Target base:", toBase_);

    root->addWidget(inputBox);

    auto *buttons = new QHBoxLayout();

    auto *convertButton = new QPushButton("Convert Number", this);
    auto *swapButton = new QPushButton("Swap Bases", this);

    buttons->addWidget(convertButton);
    buttons->addWidget(swapButton);

    root->addLayout(buttons);

    result_ = new QLabel(this);
    result_->setWordWrap(true);
    result_->setTextInteractionFlags(Qt::TextSelectableByMouse);
    root->addWidget(result_);

    auto *stepsTitle = new QLabel("Step-by-Step Solution", this);
    QFont headingFont = stepsTitle->font();
    headingFont.setBold(true);
    stepsTitle->setFont(headingFont);
    root->addWidget(stepsTitle);

    steps_ = new QTextEdit(this);
    steps_->setReadOnly(true);
    steps_->setMinimumHeight(250);
    root->addWidget(steps_);

    connect(convertButton, &QPushButton::clicked,
            this, &NumberConverterWidget::convertNumber);

    connect(number_, &QLineEdit::returnPressed,
            this, &NumberConverterWidget::convertNumber);

    connect(swapButton, &QPushButton::clicked, this, [this]() {
        int oldFrom = fromBase_->currentData().toInt();
        int oldTo = toBase_->currentData().toInt();

        fromBase_->setCurrentIndex(fromBase_->findData(oldTo));
        toBase_->setCurrentIndex(toBase_->findData(oldFrom));

        convertNumber();
    });

    connect(fromBase_, &QComboBox::currentIndexChanged,
            this, [this]() { convertNumber(); });

    connect(toBase_, &QComboBox::currentIndexChanged,
            this, [this]() { convertNumber(); });

    convertNumber();
}

void NumberConverterWidget::convertNumber()
{
    QString input = number_->text().trimmed().toUpper();

    int from = fromBase_->currentData().toInt();
    int to = toBase_->currentData().toInt();

    if (input.isEmpty()) {
        result_->setText("Please enter a number.");
        steps_->clear();
        return;
    }

    bool ok = false;
    qlonglong signedValue = input.toLongLong(&ok, from);

    if (!ok) {
        result_->setText(
            "Invalid input for base " + QString::number(from)
            + ", or the value is outside the supported signed 64-bit range."
        );

        steps_->setPlainText(
            "Check the input digits and source base.\n\n"
            "Examples:\n"
            "Base 2: 101101\n"
            "Base 8: 725\n"
            "Base 10: 123\n"
            "Base 16: 2D"
        );
        return;
    }

    const QString digits = "0123456789ABCDEF";

    bool negative = signedValue < 0;

    // Avoid signed overflow when handling the minimum signed 64-bit value.
    qulonglong magnitude = negative
        ? static_cast<qulonglong>(-(signedValue + 1)) + 1
        : static_cast<qulonglong>(signedValue);

    QString output;

    if (magnitude == 0) {
        output = "0";
    } else {
        qulonglong remaining = magnitude;

        while (remaining > 0) {
            int remainder = static_cast<int>(remaining % to);
            output.prepend(digits[remainder]);
            remaining /= to;
        }
    }

    if (negative) {
        output.prepend('-');
    }

    result_->setText(
        "Result: " + input + " (base " + QString::number(from)
        + ") = " + output + " (base " + QString::number(to) + ")"
    );

    QString explanation;

    explanation += "STEP 1: UNDERSTAND THE INPUT\n";
    explanation += "Input: " + input + "\n";
    explanation += "Source base: " + QString::number(from) + "\n";
    explanation += "Target base: " + QString::number(to) + "\n\n";

    explanation += "STEP 2: CONVERT THE INPUT TO DECIMAL\n";

    QString magnitudeText = input;

    if (negative) {
        magnitudeText.remove(0, 1);
    }

    explanation += "Each digit is multiplied by its base raised to "
                   "the digit's position, counting from the right.\n\n";

    qulonglong decimalCheck = 0;

    for (int i = 0; i < magnitudeText.size(); ++i) {
        int position = magnitudeText.size() - 1 - i;
        int digit = digits.indexOf(magnitudeText[i]);

        qulonglong power = 1;

        for (int p = 0; p < position; ++p) {
            power *= static_cast<qulonglong>(from);
        }

        qulonglong term = static_cast<qulonglong>(digit) * power;
        decimalCheck += term;

        explanation += QString("%1 x %2^%3 = %4 x %5 = %6\n")
            .arg(magnitudeText[i])
            .arg(from)
            .arg(position)
            .arg(digit)
            .arg(power)
            .arg(term);
    }

    explanation += "\nDecimal magnitude: "
        + QString::number(decimalCheck) + "\n";

    if (negative) {
        explanation += "The input is negative, so its sign is preserved.\n";
    }

    explanation += "\nSTEP 3: CONVERT DECIMAL TO BASE "
        + QString::number(to) + "\n";

    if (magnitude == 0) {
        explanation += "The value is zero, so the result is 0.\n";
    } else {
        qulonglong remaining = magnitude;
        QStringList remainders;

        while (remaining > 0) {
            qulonglong quotient = remaining / static_cast<qulonglong>(to);
            int remainder = static_cast<int>(remaining % to);

            explanation += QString("%1 / %2 = %3 remainder %4 (%5)\n")
                .arg(remaining)
                .arg(to)
                .arg(quotient)
                .arg(remainder)
                .arg(digits[remainder]);

            remainders.prepend(QString(digits[remainder]));
            remaining = quotient;
        }

        explanation += "\nRead the remainders from bottom to top:\n";
        explanation += remainders.join("");
        explanation += "\n";
    }

    explanation += "\nSTEP 4: VERIFY THE RESULT\n";

    bool verifyOk = false;
    qlonglong verifiedValue = output.toLongLong(&verifyOk, to);

    if (verifyOk && verifiedValue == signedValue) {
        explanation += "Verification successful: converting the result "
                       "back to base " + QString::number(from)
                       + " gives the original value.\n";
    } else {
        explanation += "The result was calculated, but automatic "
                       "reverse verification could not be completed.\n";
    }

    explanation += "\nFINAL ANSWER\n";
    explanation += output + " (base " + QString::number(to) + ")\n";

    steps_->setPlainText(explanation);
}