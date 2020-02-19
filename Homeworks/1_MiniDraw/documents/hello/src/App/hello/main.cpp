#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);     //  创建Qt应用
    MainWindow w;   //  创建主窗口
    w.show();   //  显示主窗口

    return a.exec();    //  将应用交给Qt托管，进入事件循环
}
