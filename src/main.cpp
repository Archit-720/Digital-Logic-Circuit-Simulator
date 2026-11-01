#include <QApplication>
#include "qt/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Digital Logic Circuit Simulator");
    window.resize(1200, 800);
    window.show();

    return app.exec();
}
