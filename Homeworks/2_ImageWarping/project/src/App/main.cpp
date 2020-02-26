#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
	QApplication a(argc, argv);
	MainWindow w;
	w.setWindowIcon(QIcon(QString("../src/App/Resources/images/favicon.png")));
	w.show();
	return a.exec();
}
