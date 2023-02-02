#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSignalMapper>
#include "mychildwnd.h"
#include "QMdiSubWindow"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initMainWindow();
    void docNew();
    void formatEnabled();
    void docOpen();
    void docSave();					//保存文档
    void docSaveAs();					//文档另存为
    void docUndo();		//撤销
    void docRedo();		//重做
    void docCut();		//剪切
    void docCopy();		//复制
    void docPaste();		//粘贴
    void textBold();				//加粗
    void textItalic();				//倾斜
    void textUnderline();			//加下画线
    void textFamily(const QString &fmly);			//字体
    void textSize(const QString &ps);				//字号
    void textAlign(QAction *act);
    void textColor();			//设置颜色
    void paraStyle(int sidx);
    void docPrint();
    void docPrintPreview();
    void printPreview(QPrinter *);

private slots:
    void refreshMenus();

    void on_actionabout_triggered();

    void on_actionaboutQt5_triggered();

    void on_actionnew_triggered();

    void on_actionclose_triggered();

    void on_actioncloseall_triggered();

    void on_actionpingpu_triggered();

    void on_actioncengdie_triggered();

    void on_actionnext_triggered();

    void on_actionpre_triggered();

    void on_actionopen_triggered();

    void on_actionsave_triggered();

    void on_actionsaveas_triggered();

    void on_actionback_triggered();

    void on_actionfront_triggered();

    void on_actioncut_triggered();

    void on_actioncopy_triggered();

    void on_actionpaste_triggered();

    void on_actionb_triggered();

    void on_actionu_triggered();

    void on_actioni_triggered();

    void on_fontComboBox_activated(const QString &arg1);

    void on_sizeComboBox_activated(const QString &arg1);

    void on_actionleft_triggered();

    void on_actionright_triggered();

    void on_actionst_triggered();

    void on_ComboBox_activated(int index);

    void on_actionc_triggered();

    void on_actionprint_triggered();

protected:

    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    MyChildWnd *activateChildWnd();
    void addSubWndListMenu();
    QSignalMapper *myWndMapper;
    QMdiSubWindow *findChildWnd(const QString &docName);
};
#endif // MAINWINDOW_H
