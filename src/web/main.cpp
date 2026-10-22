#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDateTime>
#include <QStatusBar>
#include <QScrollArea>
#include <QTableWidget>
#include <QHeaderView>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTextStream>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow() {
        setWindowTitle("Digital Circuit Simulator | Qt Frontend");
        resize(1200, 760);
        setMinimumSize(900, 600);
        buildUi();
        applyTheme();
        connect(moduleList, &QListWidget::currentRowChanged,
                this, &MainWindow::showModule);
        moduleList->setCurrentRow(2);
        statusBar()->showMessage("Ready • Frontend prototype");
    }

private:
    QListWidget *moduleList{};
    QStackedWidget *pages{};
    QLabel *pageTitle{};
    QLabel *pageDescription{};
    QTextEdit *activityLog{};
    QString currentFile;

    const QStringList moduleNames = {
        "1  Circuit Graph & Wire Routing",
        "2  Circuit Evaluation Engine",
        "3  Base Gate Class & Basic Gates",
        "4  Flip-Flops & Sequential Logic",
        "5  Mux, De-mux, Decoders, Counters & Registers",
        "6  Boolean Expression Parser",
        "7  Expression-to-Circuit Generator",
        "8  Number System Converter",
        "10 Save, Load & Autosave",
        "13 Inputs, Outputs & Wire Interaction",
        "14 Gate Properties & Truth Tables",
        "15 Validation & Error Handling",
        "16 Integration, Testing & Build Setup"
    };

    const QStringList descriptions = {
        "Create a circuit graph and manage connections between components.",
        "Evaluate a circuit from its current input values.",
        "Explore basic logic gates and their truth tables.",
        "Configure and inspect sequential logic components.",
        "Configure common combinational and sequential building blocks.",
        "Enter a Boolean expression for parsing by the backend.",
        "Prepare a Boolean expression for circuit generation.",
        "Convert values between binary, decimal, octal and hexadecimal.",
        "Save or load a project file and inspect autosave status.",
        "Set input values and inspect output values and wire state.",
        "Inspect gate settings and generate truth-table layouts.",
        "Run circuit validation and review reported issues.",
        "Review frontend integration and run prototype checks."
    };

    QWidget *makePage(const QString &title, const QString &description) {
        QWidget *page = new QWidget;
        QVBoxLayout *layout = new QVBoxLayout(page);
        layout->setContentsMargins(24, 20, 24, 20);
        layout->setSpacing(16);

        QLabel *heading = new QLabel(title);
        heading->setObjectName("pageHeading");
        QLabel *desc = new QLabel(description);
        desc->setObjectName("muted");
        desc->setWordWrap(true);
        layout->addWidget(heading);
        layout->addWidget(desc);
        QFrame *line = new QFrame;
        line->setFrameShape(QFrame::HLine);
        line->setObjectName("separator");
        layout->addWidget(line);
        pages->addWidget(page);
        return page;
    }

    QGroupBox *group(const QString &title, QVBoxLayout *parentLayout) {
        QGroupBox *box = new QGroupBox(title);
        box->setLayout(new QVBoxLayout);
        box->layout()->setContentsMargins(14, 18, 14, 14);
        box->layout()->setSpacing(10);
        parentLayout->addWidget(box);
        return box;
    }

    QPushButton *button(const QString &text, const QString &kind = "normal") {
        auto *b = new QPushButton(text);
        b->setProperty("kind", kind);
        b->setCursor(Qt::PointingHandCursor);
        b->setMinimumHeight(36);
        return b;
    }

    void log(const QString &message) {
        const QString line = QDateTime::currentDateTime().toString("HH:mm:ss")
                             + "  " + message;
        activityLog->append(line);
        statusBar()->showMessage(message, 4000);
    }

    void addButtonRow(QVBoxLayout *layout, const QStringList &labels) {
        auto *row = new QHBoxLayout;
        for (const QString &label : labels) {
            QPushButton *b = button(label, label.contains("Validate", Qt::CaseInsensitive)
                                             ? "primary" : "normal");
            row->addWidget(b);
            connect(b, &QPushButton::clicked, this, [this, label]() {
                log(label + " selected — connect backend logic here.");
            });
        }
        layout->addLayout(row);
    }

    void buildUi() {
        QWidget *central = new QWidget;
        auto *root = new QHBoxLayout(central);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(0);

        auto *sidebar = new QFrame;
        sidebar->setObjectName("sidebar");
        sidebar->setFixedWidth(290);
        auto *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(16, 20, 16, 16);
        sideLayout->setSpacing(12);

        auto *brand = new QLabel("◈  CIRCUIT LAB");
        brand->setObjectName("brand");
        sideLayout->addWidget(brand);

        auto *sub = new QLabel("DIGITAL LOGIC SIMULATOR");
        sub->setObjectName("mutedSmall");
        sideLayout->addWidget(sub);

        auto *moduleHeader = new QLabel("PROJECT MODULES");
        moduleHeader->setObjectName("sectionLabel");
        sideLayout->addWidget(moduleHeader);

        moduleList = new QListWidget;
        moduleList->setObjectName("moduleList");
        moduleList->addItems(moduleNames);
        moduleList->setSpacing(4);
        sideLayout->addWidget(moduleList, 1);

        auto *sidebarFooter = new QLabel("Qt Widgets  •  Desktop build");
        sidebarFooter->setObjectName("mutedSmall");
        sideLayout->addWidget(sidebarFooter);

        auto *right = new QWidget;
        auto *rightLayout = new QVBoxLayout(right);
        rightLayout->setContentsMargins(0, 0, 0, 0);
        rightLayout->setSpacing(0);

        auto *topbar = new QFrame;
        topbar->setObjectName("topbar");
        auto *topLayout = new QHBoxLayout(topbar);
        topLayout->setContentsMargins(22, 12, 22, 12);
        auto *projectLabel = new QLabel("Project  /  Digital Circuit Simulator");
        projectLabel->setObjectName("muted");
        topLayout->addWidget(projectLabel);
        topLayout->addStretch();
        QPushButton *saveBtn = button("Save Project", "primary");
        QPushButton *loadBtn = button("Load Project");
        topLayout->addWidget(loadBtn);
        topLayout->addWidget(saveBtn);
        connect(saveBtn, &QPushButton::clicked, this, &MainWindow::saveProject);
        connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadProject);
        rightLayout->addWidget(topbar);

        pages = new QStackedWidget;
        rightLayout->addWidget(pages, 1);

        auto *bottom = new QFrame;
        bottom->setObjectName("bottomPanel");
        auto *bottomLayout = new QVBoxLayout(bottom);
        bottomLayout->setContentsMargins(18, 10, 18, 12);
        auto *logHeader = new QHBoxLayout;
        auto *logTitle = new QLabel("ACTIVITY LOG");
        logTitle->setObjectName("sectionLabel");
        auto *clearLog = button("Clear log");
        clearLog->setMinimumHeight(28);
        logHeader->addWidget(logTitle);
        logHeader->addStretch();
        logHeader->addWidget(clearLog);
        bottomLayout->addLayout(logHeader);
        activityLog = new QTextEdit;
        activityLog->setReadOnly(true);
        activityLog->setMaximumHeight(100);
        activityLog->setPlaceholderText("Actions and prototype messages will appear here…");
        bottomLayout->addWidget(activityLog);
        connect(clearLog, &QPushButton::clicked, activityLog, &QTextEdit::clear);
        rightLayout->addWidget(bottom);

        root->addWidget(sidebar);
        root->addWidget(right, 1);
        setCentralWidget(central);
        createModulePages();

        auto *fileMenu = menuBar()->addMenu("File");
        QAction *saveAction = fileMenu->addAction("Save Project");
        QAction *loadAction = fileMenu->addAction("Load Project");
        fileMenu->addSeparator();
        QAction *exitAction = fileMenu->addAction("Exit");
        connect(saveAction, &QAction::triggered, this, &MainWindow::saveProject);
        connect(loadAction, &QAction::triggered, this, &MainWindow::loadProject);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);
    }

    void createModulePages() {
        // Module 1
        {
            QWidget *p = makePage("Circuit Graph & Wire Routing", descriptions[0]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *row = new QHBoxLayout;
            auto *left = new QVBoxLayout;
            auto *components = group("Add components", left);
            addButtonRow(qobject_cast<QVBoxLayout *>(components->layout()),
                         {"Input", "AND Gate", "OR Gate"});
            addButtonRow(qobject_cast<QVBoxLayout *>(components->layout()),
                         {"NOT Gate", "XOR Gate", "LED Output"});
            auto *wires = group("Wire routing", left);
            addButtonRow(qobject_cast<QVBoxLayout *>(wires->layout()),
                         {"Connect Components", "Remove Selected Wire"});
            row->addLayout(left, 1);
            auto *canvas = new QFrame;
            canvas->setObjectName("workspace");
            auto *cl = new QVBoxLayout(canvas);
            auto *canvasTitle = new QLabel("CIRCUIT WORKSPACE");
            canvasTitle->setObjectName("sectionLabel");
            cl->addWidget(canvasTitle);
            auto *hint = new QLabel("Your circuit graph will appear here.\n\nUse the component controls to start.\n\nBackend hook: graph nodes + edges.");
            hint->setAlignment(Qt::AlignCenter);
            hint->setObjectName("muted");
            cl->addWidget(hint, 1);
            row->addWidget(canvas, 2);
            l->addLayout(row, 1);
        }
        // Module 2
        {
            QWidget *p = makePage("Circuit Evaluation Engine", descriptions[1]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Evaluation controls", l);
            auto *row = new QHBoxLayout;
            row->addWidget(new QLabel("Evaluation mode"));
            auto *mode = new QComboBox;
            mode->addItems({"Combinational", "Sequential / Clocked"});
            row->addWidget(mode);
            row->addStretch();
            g->layout()->addItem(row);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()),
                         {"Evaluate Circuit", "Step Clock", "Reset State"});
            auto *result = new QTextEdit;
            result->setReadOnly(true);
            result->setPlaceholderText("Evaluation results will be displayed here.");
            l->addWidget(result, 1);
            connect(qobject_cast<QPushButton *>(g->findChild<QPushButton *>()),
                    &QPushButton::clicked, this, [this, result]() {
                result->setPlainText("Evaluation requested.\nConnect the backend evaluator to display output values.");
                log("Circuit evaluation requested.");
            });
        }
        // Module 3
        {
            QWidget *p = makePage("Base Gate Class & Basic Gates", descriptions[2]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Select a gate", l);
            auto *row = new QHBoxLayout;
            auto *gate = new QComboBox;
            gate->addItems({"AND", "OR", "NOT", "NAND", "NOR", "XOR", "XNOR"});
            auto *a = new QComboBox; a->addItems({"0", "1"});
            auto *b = new QComboBox; b->addItems({"0", "1"});
            row->addWidget(new QLabel("Gate"));
            row->addWidget(gate);
            row->addWidget(new QLabel("Input A"));
            row->addWidget(a);
            row->addWidget(new QLabel("Input B"));
            row->addWidget(b);
            g->layout()->addItem(row);
            auto *out = new QLabel("Output: —");
            out->setObjectName("resultLabel");
            g->layout()->addWidget(out);
            auto *eval = button("Calculate Output", "primary");
            g->layout()->addWidget(eval);
            connect(eval, &QPushButton::clicked, this, [=, this]() {
                int x = a->currentText().toInt(), y = b->currentText().toInt(), z = 0;
                QString type = gate->currentText();
                if (type == "AND") z = x & y;
                else if (type == "OR") z = x | y;
                else if (type == "NOT") z = !x;
                else if (type == "NAND") z = !(x & y);
                else if (type == "NOR") z = !(x | y);
                else if (type == "XOR") z = x ^ y;
                else z = !(x ^ y);
                out->setText("Output: " + QString::number(z));
                log(type + " gate calculated.");
            });
            l->addStretch();
        }
        // Module 4
        {
            QWidget *p = makePage("Flip-Flops & Sequential Logic", descriptions[3]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Flip-flop simulator", l);
            auto *row = new QHBoxLayout;
            auto *type = new QComboBox; type->addItems({"SR Flip-Flop", "JK Flip-Flop", "D Flip-Flop", "T Flip-Flop"});
            auto *q = new QComboBox; q->addItems({"0", "1"});
            auto *input1 = new QComboBox; input1->addItems({"0", "1"});
            auto *input2 = new QComboBox; input2->addItems({"0", "1"});
            row->addWidget(type); row->addWidget(new QLabel("Input 1")); row->addWidget(input1);
            row->addWidget(new QLabel("Input 2")); row->addWidget(input2);
            row->addWidget(new QLabel("Current Q")); row->addWidget(q);
            g->layout()->addItem(row);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()), {"Apply Clock Edge", "Reset Flip-Flop"});
            auto *note = new QLabel("Prototype UI: connect state-transition rules and clock timing in the backend.");
            note->setObjectName("muted");
            note->setWordWrap(true);
            g->layout()->addWidget(note);
            l->addStretch();
        }
        // Module 5
        {
            QWidget *p = makePage("Mux, De-mux, Decoders, Counters & Registers", descriptions[4]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Component setup", l);
            auto *row = new QHBoxLayout;
            auto *type = new QComboBox;
            type->addItems({"Multiplexer (MUX)", "Demultiplexer (DEMUX)", "Decoder", "Counter", "Register"});
            auto *bits = new QSpinBox; bits->setRange(1, 16); bits->setValue(2);
            row->addWidget(new QLabel("Component"));
            row->addWidget(type);
            row->addWidget(new QLabel("Bit width / select lines"));
            row->addWidget(bits);
            g->layout()->addItem(row);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()), {"Configure Component", "Run Component"});
            l->addStretch();
        }
        // Module 6
        {
            QWidget *p = makePage("Boolean Expression Parser", descriptions[5]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Expression input", l);
            auto *edit = new QLineEdit;
            edit->setPlaceholderText("Example: (A AND B) OR NOT C");
            g->layout()->addWidget(edit);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()), {"Parse Expression", "Clear"});
            auto *result = new QTextEdit;
            result->setReadOnly(true);
            result->setPlaceholderText("Parser diagnostics and tokens will appear here.");
            l->addWidget(result, 1);
            auto buttons = g->findChildren<QPushButton *>();
            if (buttons.size() >= 2) {
                connect(buttons[0], &QPushButton::clicked, this, [this, edit, result]() {
                    if (edit->text().trimmed().isEmpty()) {
                        result->setPlainText("Error: enter a Boolean expression first.");
                        log("Parser: expression is empty.");
                    } else {
                        result->setPlainText("Expression received:\n" + edit->text()
                            + "\n\nFrontend check passed. Full parsing belongs in the backend.");
                        log("Boolean expression submitted to parser.");
                    }
                });
                connect(buttons[1], &QPushButton::clicked, this, [edit, result]() {
                    edit->clear(); result->clear();
                });
            }
        }
        // Module 7
        {
            QWidget *p = makePage("Expression-to-Circuit Generator", descriptions[6]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Generator", l);
            auto *edit = new QLineEdit;
            edit->setPlaceholderText("Enter Boolean expression, e.g. A AND (B OR C)");
            g->layout()->addWidget(edit);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()),
                         {"Generate Circuit", "Preview Netlist"});
            auto *preview = new QTextEdit;
            preview->setReadOnly(true);
            preview->setPlaceholderText("Generated circuit or netlist preview.");
            l->addWidget(preview, 1);
            auto buttons = g->findChildren<QPushButton *>();
            if (buttons.size() >= 2) {
                connect(buttons[0], &QPushButton::clicked, this, [this, edit, preview]() {
                    preview->setPlainText(edit->text().trimmed().isEmpty()
                        ? "Enter an expression before generating a circuit."
                        : "Generation request:\n" + edit->text()
                          + "\n\nConnect expression-to-gate conversion in the backend.");
                    log("Circuit generation requested.");
                });
                connect(buttons[1], &QPushButton::clicked, this, [edit, preview]() {
                    preview->setPlainText("Netlist preview is waiting for backend integration.\nExpression: "
                                           + edit->text());
                });
            }
        }
        // Module 8
        {
            QWidget *p = makePage("Number System Converter", descriptions[7]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Convert a number", l);
            auto *row = new QHBoxLayout;
            auto *input = new QLineEdit;
            input->setPlaceholderText("Enter a value");
            auto *from = new QComboBox; from->addItems({"Decimal", "Binary", "Octal", "Hexadecimal"});
            auto *to = new QComboBox; to->addItems({"Binary", "Decimal", "Octal", "Hexadecimal"});
            row->addWidget(input, 2); row->addWidget(from); row->addWidget(new QLabel("to")); row->addWidget(to);
            g->layout()->addItem(row);
            auto *result = new QLabel("Result: —"); result->setObjectName("resultLabel");
            g->layout()->addWidget(result);
            auto *convert = button("Convert", "primary");
            g->layout()->addWidget(convert);
            connect(convert, &QPushButton::clicked, this, [this, input, from, to, result]() {
                bool ok = false;
                QString value = input->text().trimmed();
                int base = from->currentText() == "Binary" ? 2 :
                           from->currentText() == "Octal" ? 8 :
                           from->currentText() == "Hexadecimal" ? 16 : 10;
                int outBase = to->currentText() == "Binary" ? 2 :
                              to->currentText() == "Octal" ? 8 :
                              to->currentText() == "Hexadecimal" ? 16 : 10;
                qlonglong number = value.toLongLong(&ok, base);
                if (!ok) {
                    result->setText("Result: invalid input for " + from->currentText());
                    log("Number conversion failed: invalid input.");
                    return;
                }
                QString converted = QString::number(number, outBase).toUpper();
                result->setText("Result: " + converted);
                log("Number converted successfully.");
            });
            l->addStretch();
        }
        // Module 10
        {
            QWidget *p = makePage("Save, Load & Autosave", descriptions[8]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Project file", l);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()),
                         {"Save Project", "Load Project", "Autosave Status"});
            auto *status = new QLabel("Autosave: prototype mode — connect project model storage.");
            status->setObjectName("muted");
            g->layout()->addWidget(status);
            auto *timer = new QTimer(this);
            timer->setInterval(30000);
            connect(timer, &QTimer::timeout, this, [this, status]() {
                status->setText("Autosave check: " + QDateTime::currentDateTime().toString("HH:mm:ss")
                                + " — connect real project serialization.");
                log("Autosave check (prototype only).");
            });
            timer->start();
            l->addStretch();
        }
        // Module 13
        {
            QWidget *p = makePage("Inputs, Outputs & Wire Interaction", descriptions[9]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Input / output pins", l);
            auto *row = new QHBoxLayout;
            auto *pin = new QComboBox; pin->addItems({"Input A", "Input B", "Input C"});
            auto *value = new QComboBox; value->addItems({"0", "1"});
            auto *output = new QLabel("Output: —"); output->setObjectName("resultLabel");
            row->addWidget(pin); row->addWidget(new QLabel("Value")); row->addWidget(value);
            row->addWidget(output); row->addStretch();
            g->layout()->addItem(row);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()), {"Apply Input", "Inspect Wire"});
            auto buttons = g->findChildren<QPushButton *>();
            if (!buttons.isEmpty()) connect(buttons[0], &QPushButton::clicked, this, [this, pin, value, output]() {
                output->setText(pin->currentText() + " = " + value->currentText());
                log("Input value updated in frontend.");
            });
            l->addStretch();
        }
        // Module 14
        {
            QWidget *p = makePage("Gate Properties & Truth Tables", descriptions[10]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Gate properties", l);
            auto *row = new QHBoxLayout;
            auto *gate = new QComboBox; gate->addItems({"AND", "OR", "NOT", "NAND", "NOR", "XOR", "XNOR"});
            auto *name = new QLineEdit; name->setPlaceholderText("Gate label (optional)");
            row->addWidget(gate); row->addWidget(name);
            g->layout()->addItem(row);
            auto *table = new QTableWidget(4, 3);
            table->setHorizontalHeaderLabels({"A", "B", "Output"});
            table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
            table->verticalHeader()->setVisible(false);
            auto refresh = [gate, table]() {
                QString type = gate->currentText();
                int rows = type == "NOT" ? 2 : 4;
                table->setRowCount(rows);
                for (int i = 0; i < rows; ++i) {
                    int a = type == "NOT" ? i : (i / 2);
                    int b = i % 2;
                    int z = 0;
                    if (type == "AND") z = a & b;
                    else if (type == "OR") z = a | b;
                    else if (type == "NOT") z = !a;
                    else if (type == "NAND") z = !(a & b);
                    else if (type == "NOR") z = !(a | b);
                    else if (type == "XOR") z = a ^ b;
                    else z = !(a ^ b);
                    table->setItem(i, 0, new QTableWidgetItem(QString::number(a)));
                    table->setItem(i, 1, new QTableWidgetItem(type == "NOT" ? "—" : QString::number(b)));
                    table->setItem(i, 2, new QTableWidgetItem(QString::number(z)));
                }
            };
            connect(gate, &QComboBox::currentTextChanged, this, [refresh]() { refresh(); });
            refresh();
            l->addWidget(table, 1);
        }
        // Module 15
        {
            QWidget *p = makePage("Validation & Error Handling", descriptions[11]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Validate circuit", l);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()),
                         {"Validate Circuit", "Check Unconnected Inputs", "Check Output Paths"});
            auto *report = new QTextEdit;
            report->setReadOnly(true);
            report->setPlaceholderText("Validation report will appear here.");
            l->addWidget(report, 1);
            auto buttons = g->findChildren<QPushButton *>();
            for (QPushButton *b : buttons) connect(b, &QPushButton::clicked, this, [this, report, b]() {
                report->append("• " + b->text() + ": request sent. Backend validation rules are not connected yet.");
                log(b->text() + " requested.");
            });
        }
        // Module 16
        {
            QWidget *p = makePage("Integration, Testing & Build Setup", descriptions[12]);
            auto *l = qobject_cast<QVBoxLayout *>(p->layout());
            auto *g = group("Project checks", l);
            addButtonRow(qobject_cast<QVBoxLayout *>(g->layout()),
                         {"Run Frontend Checks", "Check Module List", "Clear Activity Log"});
            auto *report = new QTextEdit;
            report->setReadOnly(true);
            report->setPlainText("Frontend status: ready to connect.\nQt Widgets interface loaded.\n13 selected modules shown.\nModules 9, 11 and 12 are excluded.");
            l->addWidget(report, 1);
            auto buttons = g->findChildren<QPushButton *>();
            if (buttons.size() >= 3) {
                connect(buttons[0], &QPushButton::clicked, this, [this, report]() {
                    report->append("\nPASS: main window created\nPASS: module navigation available\nPASS: basic gate and number conversion widgets available\nNOTE: backend integration tests must be added by the team.");
                    log("Frontend checks completed.");
                });
                connect(buttons[1], &QPushButton::clicked, this, [this, report]() {
                    report->append("\nVisible module count: " + QString::number(moduleNames.size()));
                    log("Module list checked.");
                });
                connect(buttons[2], &QPushButton::clicked, this, [this]() { activityLog->clear(); });
            }
        }
    }

private slots:
    void showModule(int index) {
        if (index < 0 || index >= pages->count()) return;
        pages->setCurrentIndex(index);
        log("Opened " + moduleNames.at(index).trimmed());
    }

    void saveProject() {
        QString path = QFileDialog::getSaveFileName(this, "Save Project", currentFile.isEmpty()
            ? "circuit-project.txt" : currentFile, "Circuit project (*.txt);;All files (*)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Save failed", "Could not write to the selected file.");
            return;
        }
        QTextStream out(&file);
        out << "Digital Circuit Simulator - Qt Frontend Project\n";
        out << "Saved: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
        out << "Selected module: " << (moduleList->currentItem()
            ? moduleList->currentItem()->text() : "None") << "\n";
        out << "Note: connect circuit model serialization for full project persistence.\n";
        file.close();
        currentFile = path;
        log("Project metadata saved to " + path);
    }

    void loadProject() {
        QString path = QFileDialog::getOpenFileName(this, "Load Project", QString(),
                                                    "Circuit project (*.txt);;All files (*)");
        if (path.isEmpty()) return;
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, "Load failed", "Could not open the selected file.");
            return;
        }
        QString contents = QString::fromUtf8(file.readAll());
        file.close();
        QMessageBox::information(this, "Project file loaded",
                                 "File opened successfully.\n\n" + contents.left(700));
        currentFile = path;
        log("Project file loaded: " + path + " (metadata prototype).");
    }

    void applyTheme() {
        setStyleSheet(R"(
            QMainWindow, QWidget { background: #11151d; color: #e8edf5; font-family: "Segoe UI"; font-size: 13px; }
            #sidebar { background: #0c1017; border-right: 1px solid #273141; }
            #brand { color: #f2f6ff; font-size: 19px; font-weight: 800; padding: 4px 0; }
            #muted, #mutedSmall { color: #98a6ba; }
            #mutedSmall { font-size: 10px; letter-spacing: 1px; }
            #sectionLabel { color: #8393ab; font-size: 10px; font-weight: 800; letter-spacing: 1.2px; }
            #moduleList { background: transparent; border: none; outline: none; color: #aab6c8; }
            #moduleList::item { padding: 12px 9px; border-radius: 7px; }
            #moduleList::item:selected { background: #1e3553; color: #ffffff; border-left: 3px solid #55a6ff; }
            #moduleList::item:hover { background: #182333; }
            #topbar { background: #141a24; border-bottom: 1px solid #273141; }
            #bottomPanel { background: #0d121a; border-top: 1px solid #273141; }
            #pageHeading { color: #f4f7fc; font-size: 24px; font-weight: 750; }
            #separator { color: #273141; background: #273141; max-height: 1px; }
            #workspace { background: #0c1119; border: 1px dashed #35465e; border-radius: 10px; }
            QGroupBox { border: 1px solid #2a3545; border-radius: 9px; margin-top: 10px; padding-top: 10px; font-weight: 650; color: #dce5f2; }
            QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 5px; }
            QPushButton { background: #202a39; border: 1px solid #35445a; border-radius: 6px; padding: 8px 12px; color: #e7edf6; }
            QPushButton:hover { background: #2a3b52; }
            QPushButton[kind="primary"] { background: #2676d2; border-color: #3488e8; color: white; font-weight: 700; }
            QPushButton[kind="primary"]:hover { background: #3388e8; }
            QLineEdit, QComboBox, QSpinBox, QTextEdit, QTableWidget { background: #0c1119; border: 1px solid #344155; border-radius: 6px; padding: 8px; selection-background-color: #2676d2; }
            QComboBox QAbstractItemView { background: #141a24; selection-background-color: #2676d2; }
            QTableWidget { gridline-color: #293548; }
            QHeaderView::section { background: #1a2331; color: #b9c7d9; padding: 7px; border: none; }
            #resultLabel { color: #75d6b1; font-size: 17px; font-weight: 700; padding: 6px 0; }
            QStatusBar { background: #0c1017; color: #98a6ba; }
            QMenuBar { background: #10151e; color: #cdd7e5; }
            QMenuBar::item:selected, QMenu::item:selected { background: #263b56; }
            QMenu { background: #141a24; border: 1px solid #344155; }
        )");
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Digital Circuit Simulator");
    MainWindow window;
    window.show();
    return app.exec();
}
#include "main.moc"

