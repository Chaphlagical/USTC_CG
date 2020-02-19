#include "minidraw.h"

//	ʵ�������н���ͼ�ؼ���ʵ��������Init������
//	Init��������MiniDraw�Ĺ��캯���б�����
MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget();	//	ʵ����viewWidget�ؼ�����
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();
	setCentralWidget(view_widget_);	//	��viewWidget�ؼ�����Ϊ�����ڵ�����λ��
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
