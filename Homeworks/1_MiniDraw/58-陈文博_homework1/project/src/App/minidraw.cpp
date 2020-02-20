#include "minidraw.h"]
#include <iostream>

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
	Action_About->setIcon(QIcon(QString("../src/img/about.png")));	//	添加图标
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);
	Action_Line = new QAction(tr("&Line"), this);
	Action_Line->setIcon(QIcon(QString("../src/img/line.png")));
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rectangle"), this);
	Action_Rect->setIcon(QIcon(QString("../src/img/rect.png")));
	connect(Action_Rect, &QAction::triggered, view_widget_, &ViewWidget::setRect);

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	Action_Ellipse->setIcon(QIcon(QString("../src/img/ellipse.png")));
	connect(Action_Ellipse, &QAction::triggered, view_widget_, &ViewWidget::setEllipse);

	Action_Freedraw = new QAction(tr("&Freedraw"), this);
	Action_Freedraw->setIcon(QIcon(QString("../src/img/freedraw.png")));
	connect(Action_Freedraw, &QAction::triggered, view_widget_, &ViewWidget::setFreedraw);

	Action_Polygon = new QAction(tr("&Polygon"), this);
	Action_Polygon->setIcon(QIcon(QString("../src/img/polygon.png")));
	connect(Action_Polygon, &QAction::triggered, view_widget_, &ViewWidget::setPolygon);

	Action_Undo = new QAction(tr("&Undo"), this);
	Action_Undo->setIcon(QIcon(QString("../src/img/undo.png")));
	connect(Action_Undo, &QAction::triggered, view_widget_, &ViewWidget::undo);

	Action_Line_Color = new QAction(tr("&Line Color"), this);
	Action_Line_Color->setIcon(QIcon(QString("../src/img/color.png")));
	connect(Action_Line_Color, &QAction::triggered, view_widget_, &ViewWidget::setLineColor);

	Action_Fill_Color = new QAction(tr("&Fill Color"), this);
	Action_Fill_Color->setIcon(QIcon(QString("../src/img/fill.png")));
	connect(Action_Fill_Color, &QAction::triggered, view_widget_, &ViewWidget::setFillColor);

	Action_Width = new QAction(tr("&Line Width"), this);
	Action_Width->setIcon(QIcon(QString("../src/img/width.png")));
	connect(Action_Width, &QAction::triggered, view_widget_, &ViewWidget::setWidth);

	Action_Save = new QAction(tr("&Save"), this);
	Action_Save->setIcon(QIcon(QString("../src/img/save.png")));
	connect(Action_Save, &QAction::triggered, view_widget_, &ViewWidget::save);
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
	pToolBar->addAction(Action_Save);
}

void MiniDraw::Creat_Menu()
{
	pMenu = menuBar()->addMenu(tr("&Figure Tool"));	pMenu->setIcon(QIcon(QString("../src/img/tool.png")));	pMenu->addAction(Action_About);	pMenu->addAction(Action_Line);	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Freedraw);
	pMenu->addAction(Action_Polygon);
	pMenu->addAction(Action_Undo);
	pMenu->addAction(Action_Line_Color);
	pMenu->addAction(Action_Fill_Color);
	pMenu->addAction(Action_Width);
	pMenu->addAction(Action_Save);
}

void MiniDraw::AboutBox()
{
	QMessageBox::about(this, tr("About"), tr("MiniDraw \nEnjoy yourself !\n by Chaf"));
}

MiniDraw::~MiniDraw()
{

}
