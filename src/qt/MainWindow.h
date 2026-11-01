#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QListWidget;
class QStackedWidget;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QListWidget *navigation_;
    QStackedWidget *pages_;
    QLabel *pageTitle_;
    QLabel *pageDescription_;

    void showPage(int index);
};

#endif
