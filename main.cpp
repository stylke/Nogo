#include "mainwindow.h"

#include <QApplication>

extern int TIME_LIMIT;
extern int LINE_NUM;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
