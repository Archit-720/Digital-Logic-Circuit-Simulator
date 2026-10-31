#include "MainWindow.h"

#include "CircuitCanvas.h"
#include "ExpressionWidget.h"
#include "TruthTableWidget.h"
#include "BooleanSimplifierWidget.h"
#include "KMapWidget.h"
#include "FlipFlopConversionWidget.h"
#include "CounterDesignerWidget.h"
#include "NumberConverterWidget.h"

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      navigation_(new QListWidget(this)),
      pages_(new QStackedWidget(this)),
      pageTitle_(new QLabel(this)),
      pageDescription_(new QLabel(this))
{
    setWindowTitle("Digital Logic Circuit Simulator");
    resize(1440, 900);
    setMinimumSize(1050, 700);

    QStringList moduleNames = {
        "Circuit Workspace",
        "Expression Lab",
        "Truth Table",
        "Boolean Simplifier",
        "Karnaugh Map",
        "Flip-Flop Converter",
        "Counter Designer",
        "Number Converter"
    };

    QStringList descriptions = {
        "Build and inspect digital logic circuits.",
        "Evaluate Boolean expressions and explore their logic.",
        "Generate and inspect truth tables.",
        "Simplify Boolean expressions step by step.",
        "Group minterms and find simplified logic functions.",
        "Explore conversions between flip-flop types.",
        "Design counter sequences and excitation tables.",
        "Convert numbers between different bases."
    };

    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *sidebar = new QFrame(central);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(250);

    auto *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(16, 22, 16, 18);
    sidebarLayout->setSpacing(12);

    auto *brand = new QLabel("LOGIC<span>DESIGN</span>", sidebar);
    brand->setObjectName("brand");
    brand->setTextFormat(Qt::RichText);

    auto *brandSubtitle = new QLabel("DIGITAL DESIGN", sidebar);
    brandSubtitle->setObjectName("brandSubtitle");

    auto *navigationLabel = new QLabel("WORKSPACE MODULES", sidebar);
    navigationLabel->setObjectName("sectionLabel");

    navigation_->setObjectName("navigation");
    navigation_->setSpacing(5);
    navigation_->setFrameShape(QFrame::NoFrame);
    navigation_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    navigation_->setVerticalScrollMode(QListWidget::ScrollPerPixel);

    for (const QString &name : moduleNames)
        navigation_->addItem(name);

    auto *sidebarBottom = new QLabel(
        "DIGITAL LOGIC CIRCUIT SIMULATOR\nDesktop Edition  •  v1.0",
        sidebar
    );
    sidebarBottom->setObjectName("sidebarBottom");

    sidebarLayout->addWidget(brand);
    sidebarLayout->addWidget(brandSubtitle);
    sidebarLayout->addSpacing(25);
    sidebarLayout->addWidget(navigationLabel);
    sidebarLayout->addWidget(navigation_, 1);
    sidebarLayout->addWidget(sidebarBottom);

    auto *content = new QWidget(central);
    content->setObjectName("contentArea");

    auto *contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(25, 20, 25, 20);
    contentLayout->setSpacing(16);

    auto *topBar = new QFrame(content);
    topBar->setObjectName("topBar");
    topBar->setMinimumHeight(76);

    auto *topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(20, 12, 20, 12);

    auto *headingLayout = new QVBoxLayout();
    headingLayout->setSpacing(4);

    pageTitle_->setObjectName("pageTitle");
    pageTitle_->setText(moduleNames.first());

    pageDescription_->setObjectName("pageDescription");
    pageDescription_->setText(descriptions.first());

    headingLayout->addWidget(pageTitle_);
    headingLayout->addWidget(pageDescription_);

    auto *workspaceBadge = new QLabel("●  DESKTOP WORKSPACE", topBar);
    workspaceBadge->setObjectName("workspaceBadge");

    topBarLayout->addLayout(headingLayout, 1);
    topBarLayout->addWidget(workspaceBadge, 0, Qt::AlignRight);

    auto *pageContainer = new QFrame(content);
    pageContainer->setObjectName("pageContainer");

    auto *pageLayout = new QVBoxLayout(pageContainer);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->setSpacing(0);

    pages_->setObjectName("modulePages");

    pages_->addWidget(new CircuitCanvas(pages_));
    pages_->addWidget(new ExpressionWidget(pages_));
    pages_->addWidget(new TruthTableWidget(pages_));
    pages_->addWidget(new BooleanSimplifierWidget(pages_));
    pages_->addWidget(new KMapWidget(pages_));
    pages_->addWidget(new FlipFlopConversionWidget(pages_));
    pages_->addWidget(new CounterDesignerWidget(pages_));
    pages_->addWidget(new NumberConverterWidget(pages_));

    pageLayout->addWidget(pages_);

    contentLayout->addWidget(topBar);
    contentLayout->addWidget(pageContainer, 1);

    rootLayout->addWidget(sidebar);
    rootLayout->addWidget(content, 1);

    setCentralWidget(central);

    setStyleSheet(R"(
        QMainWindow {
            background: #101820;
        }

        QWidget {
            color: #E6EDF3;
            font-family: "Segoe UI";
            font-size: 10pt;
        }

        #sidebar {
            background: #15232D;
            border-right: 1px solid #293B46;
        }

        #brand {
            color: #F0F6FC;
            font-size: 25pt;
            font-weight: 800;
            letter-spacing: 1px;
        }

        #brand span {
            color: #43D6B5;
        }

        #brandSubtitle {
            color: #8296A4;
            font-size: 8pt;
            letter-spacing: 2px;
        }

        #sectionLabel {
            color: #8296A4;
            font-size: 8pt;
            font-weight: 700;
            padding-left: 8px;
        }

        #navigation {
            background: transparent;
            border: none;
            outline: none;
            color: #B8C6D0;
            font-size: 10pt;
        }

        #navigation::item {
            min-height: 42px;
            padding: 0 12px;
            border-radius: 7px;
            margin: 1px 0;
        }

        #navigation::item:hover {
            background: #203640;
            color: #FFFFFF;
        }

        #navigation::item:selected {
            background: #17483F;
            color: #61E6C4;
            border-left: 3px solid #43D6B5;
            font-weight: 700;
        }

        #sidebarBottom {
            color: #8296A4;
            font-size: 8pt;
            padding: 12px 8px 0 8px;
            border-top: 1px solid #293B46;
        }

        #contentArea {
            background: #101820;
        }

        #topBar, #pageContainer {
            background: #192630;
            border: 1px solid #293B46;
            border-radius: 10px;
        }

        #pageTitle {
            color: #F0F6FC;
            font-size: 18pt;
            font-weight: 700;
        }

        #pageDescription {
            color: #91A4B1;
            font-size: 9pt;
        }

        #workspaceBadge {
            color: #61E6C4;
            background: #173B37;
            border: 1px solid #24574E;
            border-radius: 6px;
            padding: 9px 12px;
            font-size: 8pt;
            font-weight: 700;
        }

        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background: #111D26;
            color: #E6EDF3;
            border: 1px solid #344955;
            border-radius: 5px;
            padding: 7px 9px;
            min-height: 20px;
            selection-background-color: #247B6A;
        }

        QLineEdit:focus, QComboBox:focus,
        QSpinBox:focus, QDoubleSpinBox:focus {
            border: 1px solid #43D6B5;
        }

        QPushButton {
            background: #263C48;
            color: #E6EDF3;
            border: 1px solid #3A515E;
            border-radius: 6px;
            padding: 9px 14px;
            min-height: 20px;
            font-weight: 600;
        }

        QPushButton:hover {
            background: #31515D;
            border-color: #43D6B5;
        }

        QPushButton:pressed {
            background: #1A6B5B;
        }

        QTableWidget, QTableView {
            background: #121F28;
            alternate-background-color: #192A35;
            color: #E6EDF3;
            gridline-color: #2B404B;
            border: 1px solid #2B404B;
            border-radius: 6px;
            selection-background-color: #245D53;
            selection-color: #FFFFFF;
        }

        QHeaderView::section {
            background: #20323D;
            color: #B9D2D8;
            border: none;
            border-right: 1px solid #344955;
            border-bottom: 1px solid #344955;
            padding: 8px;
            font-weight: 700;
        }

        QPlainTextEdit, QTextEdit {
            background: #111D26;
            color: #DCE8EE;
            border: 1px solid #2B404B;
            border-radius: 6px;
            padding: 8px;
            selection-background-color: #247B6A;
        }

        QGroupBox {
            background: #192630;
            border: 1px solid #344955;
            border-radius: 8px;
            margin-top: 13px;
            padding: 12px 9px 9px 9px;
            font-weight: 700;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 5px;
            color: #61DCC0;
        }

        QLabel {
            background: transparent;
        }

        QScrollArea {
            background: transparent;
            border: none;
        }

        QScrollBar:vertical {
            background: #14212A;
            width: 10px;
            margin: 0;
        }

        QScrollBar::handle:vertical {
            background: #3A535E;
            min-height: 25px;
            border-radius: 4px;
        }

        QScrollBar::handle:vertical:hover {
            background: #43A995;
        }

        QScrollBar:horizontal {
            background: #14212A;
            height: 10px;
            margin: 0;
        }

        QScrollBar::handle:horizontal {
            background: #3A535E;
            min-width: 25px;
            border-radius: 4px;
        }

        QToolTip {
            background: #243943;
            color: #FFFFFF;
            border: 1px solid #43D6B5;
            padding: 5px;
        }
    )");

    connect(navigation_, &QListWidget::currentRowChanged,
            this, &MainWindow::showPage);

    navigation_->setCurrentRow(0);
}

void MainWindow::showPage(int index)
{
    if (index < 0 || index >= pages_->count())
        return;

    static const QStringList titles = {
        "Circuit Workspace",
        "Expression Lab",
        "Truth Table",
        "Boolean Simplifier",
        "Karnaugh Map Solver",
        "Flip-Flop Converter",
        "Counter Designer",
        "Number System Converter"
    };

    static const QStringList descriptions = {
        "Build and inspect digital logic circuits.",
        "Evaluate Boolean expressions and explore their logic.",
        "Generate and inspect truth tables.",
        "Simplify Boolean expressions step by step.",
        "Group minterms and find simplified logic functions.",
        "Explore conversions between flip-flop types.",
        "Design counter sequences and excitation tables.",
        "Convert numbers between different bases."
    };

    pages_->setCurrentIndex(index);
    pageTitle_->setText(titles[index]);
    pageDescription_->setText(descriptions[index]);
}
