#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);     //  ����QtӦ��
    MainWindow w;   //  ����������
    w.show();   //  ��ʾ������

    return a.exec();    //  ��Ӧ�ý���Qt�йܣ������¼�ѭ��
}
