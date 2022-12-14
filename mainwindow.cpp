#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QFile>
#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //ui->graphicsView->setViewport(new QOpenGLWidget());
    //QGraphicsScene* scene = new QGraphicsScene;
    ////向容器中添加文件路径为fileName（QString类型）的文件
    //scene->addPixmap(QPixmap("./gqj.jpeg"));
    //scene->addWidget(ui->yuvWidget);
    ////借助graphicsView（QGraphicsView类）控件显示容器的内容
    //ui->graphicsView->setScene(scene);

    {
        auto t = new QTimer(this);
        auto f = new QFile("./yuv/1.yuv", this);
    
        auto ba = new QByteArray(176 * 144 * 1.5, 0);

        bool bOpen = f->open(QIODevice::ReadOnly);
        qDebug(" c=%d", ba->count());
        connect(t, &QTimer::timeout, this, [=]() {
            //
            const int w = 176, h = 144;
            if (f->read(ba->data(), ba->count()) <= 0) {//定时在f中读取176 * 144 * 1.5个数据到ba中
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


void MainWindow::on_pushButton_clicked()
{
    QMessageBox::information(this, "Tips", "Box");
}
