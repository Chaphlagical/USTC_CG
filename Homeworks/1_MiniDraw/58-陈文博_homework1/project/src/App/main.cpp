#include "minidraw.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv);
	MiniDraw w;
	w.setWindowIcon(QIcon(QString("../src/img/favicon.png")));	//	设置主窗口图标
	w.show();
	return a.exec();
}
