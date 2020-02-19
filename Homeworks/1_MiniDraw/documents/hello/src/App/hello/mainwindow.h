#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    Ui::MainWindow *ui;// 由*.ui 文件定义，Qt会自动生成相关的u_*.h文件

    QAction* hello_world_action_;   //  声明动作
    QMenu* main_menu_;  //  声明菜单
    QToolBar* main_toolbar_;    //声明工具栏
    void CreateButtons();   //声明函数

private slots:
    void HelloWorld();
};

#endif // MAINWINDOW_H
