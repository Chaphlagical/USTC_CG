#include "minidraw.h"

//	实例代码中将绘图控件的实例化放在Init函数中
//	Init函数将在MiniDraw的构造函数中被调用
MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget();	//	实例化viewWidget控件窗口
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();
	setCentralWidget(view_widget_);	//	将viewWidget控件设置为主窗口的中心位置
}

void MiniDraw::Creat_Action()
{
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);
	Action_Line = new QAction(tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rect"), this);
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	connect(Action_Ellipse, &QAction::triggered, view_widget_, &ViewWidget::setEllipse);
}

void MiniDraw::Creat_ToolBar()
{
	pToolBar = addToolBar(tr("&Main"));	pToolBar->addAction(Action_About);	pToolBar->addAction(Action_Line);	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
}

void MiniDraw::Creat_Menu()
{
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));	pMenu->addAction(Action_About);	pMenu->addAction(Action_Line);	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
}

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw"));
}

MiniDraw::~MiniDraw()
{

}
