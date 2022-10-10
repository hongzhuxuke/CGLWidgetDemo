#include "mainwindow.h"

#include <QApplication>
#include "myglwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    myGlWidget w;
    w.setFixedSize(QSize(800, 800));
    w.show();
    return a.exec();
}
