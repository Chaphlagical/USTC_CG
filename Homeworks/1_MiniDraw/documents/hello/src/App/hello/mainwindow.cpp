#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qmessagebox.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);	//	UI初始化，就在ui_mainwindow.h 里定义了，必须最先调用
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
	//	创建动作，标签为“Hello World”
	//	&的作用：设置此动作的快捷键为标签字符串的首字母，此例为'H'
	hello_world_action_ = new QAction(tr("&Hello world"), this);
	connect(hello_world_action_, &QAction::triggered, this, &MainWindow::HelloWorld);

	//	创建菜单，标签为Main
	main_menu_ = menuBar()->addMenu(tr("&Main"));
	
	//	将动作Hello world
	main_menu_->addAction(hello_world_action_);
	main_toolbar_ = addToolBar(tr("&Main"));
	main_toolbar_->addAction(hello_world_action_);
}
