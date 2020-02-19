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

	Action_Freedraw = new QAction(tr("&Freedraw"), this);
	connect(Action_Freedraw, &QAction::triggered, view_widget_, &ViewWidget::setFreedraw);

	Action_Polygon = new QAction(tr("&Polygon"), this);
	connect(Action_Polygon, &QAction::triggered, view_widget_, &ViewWidget::setPolygon);

	Action_Undo = new QAction(tr("&Undo"), this);
	connect(Action_Undo, &QAction::triggered, view_widget_, &ViewWidget::undo);

	Action_Line_Color = new QAction(tr("&Line Color"), this);
	connect(Action_Line_Color, &QAction::triggered, view_widget_, &ViewWidget::setLineColor);

	Action_Fill_Color = new QAction(tr("&Fill Color"), this);
	connect(Action_Fill_Color, &QAction::triggered, view_widget_, &ViewWidget::setFillColor);

	Action_Width = new QAction(tr("&Line Width"), this);
	connect(Action_Width, &QAction::triggered, view_widget_, &ViewWidget::setWidth);
}

void MiniDraw::Creat_ToolBar()
{
	pToolBar = addToolBar(tr("&Main"));	pToolBar->addAction(Action_About);	pToolBar->addAction(Action_Line);	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Freedraw);
	pToolBar->addAction(Action_Polygon);
	pToolBar->addAction(Action_Undo);
	pToolBar->addAction(Action_Line_Color);
	pToolBar->addAction(Action_Fill_Color);
	pToolBar->addAction(Action_Width);
}

void MiniDraw::Creat_Menu()
{
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));	pMenu->addAction(Action_About);	pMenu->addAction(Action_Line);	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Freedraw);
	pMenu->addAction(Action_Polygon);
	pMenu->addAction(Action_Undo);
	pMenu->addAction(Action_Line_Color);
	pMenu->addAction(Action_Fill_Color);
	pMenu->addAction(Action_Width);
}

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw \n by Chaf"));
}

MiniDraw::~MiniDraw()
{

}
