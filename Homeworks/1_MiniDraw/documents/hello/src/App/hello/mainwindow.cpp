#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);	//	UI��ʼ��������ui_mainwindow.h �ﶨ���ˣ��������ȵ���
	CreateButtons();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::HelloWorld() {
	QMessageBox::about(this, tr("Hello world"),
		tr("Well done! Go ahead."));
}

void MainWindow::CreateButtons()
{
	//	������������ǩΪ��Hello World��
	//	&�����ã����ô˶����Ŀ�ݼ�Ϊ��ǩ�ַ���������ĸ������Ϊ'H'
	hello_world_action_ = new QAction(tr("&Hello world"), this);
	connect(hello_world_action_, &QAction::triggered, this, &MainWindow::HelloWorld);

	//	�����˵�����ǩΪMain
	main_menu_ = menuBar()->addMenu(tr("&Main"));
	
	//	������Hello world
	main_menu_->addAction(hello_world_action_);
	main_toolbar_ = addToolBar(tr("&Main"));
	main_toolbar_->addAction(hello_world_action_);
}
