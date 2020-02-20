#ifndef MINIDRAW_H
#define MINIDRAW_H

#include <ui_minidraw.h>
#include <viewwidget.h>

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qtoolbar.h> 

class MiniDraw : public QMainWindow
{
	Q_OBJECT

public:
	MiniDraw(QWidget *parent = 0);
	~MiniDraw();

	QMenu *pMenu;
	QToolBar *pToolBar;
	QAction *Action_About;
	QAction *Action_Line;
	QAction *Action_Rect;
	QAction* Action_Ellipse;
	QAction* Action_Freedraw;
	QAction* Action_Polygon;
	QAction* Action_Undo;
	QAction* Action_Line_Color;
	QAction* Action_Fill_Color;
	QAction* Action_Width;
	QAction* Action_Save;

	void Creat_Menu();
	void Creat_ToolBar();
	void Creat_Action();

	void AboutBox();

private:
	Ui::MiniDrawClass ui;
	ViewWidget*	view_widget_;	//	ÉùÃ÷viewWidgetÖ¸Õë
};

#endif // MINIDRAW_H
