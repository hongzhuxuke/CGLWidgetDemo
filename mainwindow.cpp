#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    {
        auto t = new QTimer(this);
        auto f = new QFile("./yuv/1.yuv", this);
    
        auto ba = new QByteArray(176 * 144 * 1.5, 0);

        bool bOpen = f->open(QIODevice::ReadOnly);
        qDebug(" c=%d", ba->count());
        connect(t, &QTimer::timeout, this, [=]() {
            //
            const int w = 176, h = 144;
            if (f->read(ba->data(), ba->count()) <= 0) {
                f->seek(0);
                if (f->read(ba->data(), ba->count()) <= 0) {
                    qDebug("error again");
                    t->stop();
                    return;
                }
            }
            ui->yuvWidget->slotShowYuv((uchar*)(ba->data()), w, h);
            });
        t->setInterval(25);
        t->start();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

