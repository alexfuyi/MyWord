#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QFontDatabase"
#include "QComboBox"
#include "QTextEdit"
#include "QSignalMapper"
#include "QMessageBox"
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initMainWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionabout_triggered()
{
    QMessageBox::about(this, tr("关于"), tr("这是一个基于 Qt5 实现的字处理软件\n具备类似 微软 Office Word 的功能。"));

}

void MainWindow::on_actionaboutQt5_triggered()
{
        QMessageBox::aboutQt(NULL, "关于 Qt 5");
}

void MainWindow::initMainWindow()
{
    QFontDatabase fontdb;
    foreach(int fontsize, fontdb.standardSizes())
        ui->sizeComboBox->addItem(QString::number(fontsize));
    ui->sizeComboBox->setCurrentIndex(ui->sizeComboBox->findText(QString::number(QApplication::font().pointSize())));
    refreshMenus();
       connect(ui->mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT (refreshMenus()));
   //添加子窗口菜单项列表
       myWndMapper = new QSignalMapper(this);		//创建信号映射器
       connect(myWndMapper, SIGNAL(mapped(QWidget*)), this, SLOT(setActiveSubWindow(QWidget*)));
       addSubWndListMenu();
       connect(ui->menu_5, SIGNAL(aboutToShow()), this, SLOT(addSubWndListMenu()));
       QActionGroup *alignGroup = new QActionGroup(this);
       alignGroup->addAction(ui->actionleft);
       alignGroup->addAction(ui->actionright);
       alignGroup->addAction(ui->actionst);
       ui->actionleft->setChecked(true);

}

void MainWindow::formatEnabled()
{
    ui->actionb->setEnabled(true);
    ui->actioni->setEnabled(true);
    ui->actionu->setEnabled(true);
    ui->actionleft->setEnabled(true);
    ui->actionst->setEnabled(true);
    ui->actionright->setEnabled(true);
    ui->actionc->setEnabled(true);
}


void MainWindow::docNew()
{
    MyChildWnd *childWnd = new MyChildWnd();		//创建MyChildWnd部件
    //向多文档区域添加子窗口，childWnd为中心部件
    ui->mdiArea->addSubWindow((QWidget*)childWnd);
    //根据QTextEdit类是否可以复制信号，设置剪切、复制动作是否可用
    connect(childWnd, SIGNAL(copyAvailable(bool)),ui->actioncut,SLOT(setEnabled (bool)));
    connect(childWnd,SIGNAL(copyAvailable(bool)),ui->actioncopy,SLOT(setEnabled (bool)));
    childWnd->newDoc();
    childWnd->show();
    //使“格式”主菜单下各菜单项及其对应的工具按钮变为可用
    formatEnabled();
}


void MainWindow::on_actionnew_triggered()
{
    this->docNew();
}

MyChildWnd *MainWindow::activateChildWnd()
{
    //若有活动文档窗口则将其内的中心部件转换为MyChildWnd类型；若没有则直接返回0
    QMdiSubWindow *actSubWnd = ui->mdiArea->activeSubWindow();
    if(actSubWnd)
        return (MyChildWnd*)(actSubWnd->widget());
    else
        return 0;
}

void MainWindow::refreshMenus()
{
    //至少有一个文档子窗口打开的情况
    bool hasChild = (activateChildWnd() != 0);
    ui->actionsave->setEnabled(hasChild);
    ui->actionsaveas->setEnabled(hasChild);
    ui->actionprint->setEnabled(hasChild);
    ui->actionpaste->setEnabled(hasChild);
    ui->actionclose->setEnabled(hasChild);
    ui->actioncloseall->setEnabled(hasChild);
    ui->actioni->setEnabled(hasChild);
    ui->actioncengdie->setEnabled(hasChild);
    ui->actionnext->setEnabled(hasChild);
    ui->actionpre->setEnabled(hasChild);
    //文档已打开并且其中有内容被选中的情况
    bool hasSelect = (activateChildWnd() && activateChildWnd()->textCursor().hasSelection());
    ui->actioncut->setEnabled(hasSelect);
    ui->actioncopy->setEnabled(hasSelect);
    ui->actionb->setEnabled(hasSelect);
    ui->actioni->setEnabled(hasSelect);
    ui->actionu->setEnabled(hasSelect);
    ui->actionleft->setEnabled(hasSelect);
    ui->actionst->setEnabled(hasSelect);
    ui->actionright->setEnabled(hasSelect);
    ui->actionc->setEnabled(hasSelect);
}

void MainWindow::addSubWndListMenu()
{
    //首先清空原“窗口”主菜单，然后再添加各菜单项
    ui->menu_5->clear();
    ui->menu_5->addAction(ui->actionclose);
    ui->menu_5->addAction(ui->actioncloseall);
    ui->menu_5->addSeparator();
    ui->menu_5->addAction(ui->actionpingpu);
    ui->menu_5->addAction(ui->actioncengdie);
    ui->menu_5->addSeparator();
    ui->menu_5->addAction(ui->actionnext);
    ui->menu_5->addAction(ui->actionpre);
    QList<QMdiSubWindow *> wnds = ui->mdiArea->subWindowList();
    if (!wnds.isEmpty()) ui->menu_5->addSeparator();
                                            //如果有活动子窗口，则显示分隔条
    //遍历各子窗口，显示所有当前已打开的文档子窗口项
    for(int i=0; i<wnds.size(); ++i)
    {
        MyChildWnd *childwnd=qobject_cast<MyChildWnd *>(wnds.at(i)->widget());
        QString menuitem_text;
        if(i<9)
        {
            menuitem_text=tr("&%1 %2").arg(i+1).arg(childwnd->getCurDocName());
        } else {
            menuitem_text=tr("%1 %2").arg(i+1).arg(childwnd->getCurDocName());
        }
        //添加子窗口菜单项，设置其可选
        QAction *menuitem_act = ui->menu_5->addAction(menuitem_text);
        menuitem_act->setCheckable(true);
        //将当前活动的子窗口设为勾选状态
        menuitem_act->setChecked(childwnd == activateChildWnd());
        //关联菜单项的触发信号到信号映射器的map()槽，该槽会发送mapped()信号
        connect(menuitem_act, SIGNAL(triggered()), myWndMapper, SLOT(map()));
        //将菜单项与相应的窗口部件进行映射，在发送mapped()信号时就会以这个窗口部件为参数
        myWndMapper->setMapping(menuitem_act, wnds.at(i));
    }
    formatEnabled();							//使“字体”菜单下的功能可用
}


void MainWindow::on_actionclose_triggered()
{
    ui->mdiArea->closeActiveSubWindow();
}

void MainWindow::on_actioncloseall_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}

void MainWindow::on_actionpingpu_triggered()
{
    ui->mdiArea->tileSubWindows();
}

void MainWindow::on_actioncengdie_triggered()
{
     ui->mdiArea->cascadeSubWindows();
}

void MainWindow::on_actionnext_triggered()
{
    ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionpre_triggered()
{
    ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->mdiArea->closeAllSubWindows();
    if (ui->mdiArea->currentSubWindow())
    {
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::docOpen()
{
    QString docName = QFileDialog::getOpenFileName(this, tr("打开"), QString(),
 tr("HTML 文档 (*.htm *.html);;所有文件 (*.*)"));
    if (!docName.isEmpty())
    {
        QMdiSubWindow *exist = findChildWnd(docName);
        if (exist)
        {
            ui->mdiArea->setActiveSubWindow(exist);
            return;
        }
        MyChildWnd *childwnd = new MyChildWnd;
        ui->mdiArea->addSubWindow(childwnd);
        connect(childwnd, SIGNAL(copyAvailable(bool)), ui->actioncut, SLOT(setEnabled(bool)));
        connect(childwnd, SIGNAL(copyAvailable(bool)), ui->actioncopy,SLOT(setEnabled(bool)));
        if (childwnd->loadDoc(docName))
        {
            statusBar()->showMessage(tr("文档已打开"), 2000);
            childwnd->show();
            formatEnabled();				//使“字体”菜单下的功能可用
        } else {
            childwnd->close();
        }
    }
}

QMdiSubWindow *MainWindow::findChildWnd(const QString &docName)
{
    QString canonicalFilePath = QFileInfo(docName).canonicalFilePath();
    foreach(QMdiSubWindow *wnd, ui->mdiArea->subWindowList())
    {
        MyChildWnd *childwnd = qobject_cast<MyChildWnd *>(wnd->widget());
        if(childwnd->myCurDocPath == canonicalFilePath) return wnd;
    }
    return 0;
}


void MainWindow::on_actionopen_triggered()
{
    this->docOpen();
}

void MainWindow::docSave()
{
    if(activateChildWnd() && activateChildWnd()->saveDoc())
        statusBar()->showMessage(tr("保存成功"), 2000);
}
void MainWindow::docSaveAs()
{
    if(activateChildWnd() && activateChildWnd()->saveAsDoc())
        statusBar()->showMessage(tr("保存成功"), 2000);
}


void MainWindow::on_actionsave_triggered()
{
    docSave();
}

void MainWindow::on_actionsaveas_triggered()
{
    docSaveAs();
}

void MainWindow::docUndo()
{
    if(activateChildWnd()) activateChildWnd()->undo();		//撤销
}

void MainWindow::docRedo()
{
    if(activateChildWnd()) activateChildWnd()->redo();		//重做
}

void MainWindow::docCut()
{
    if(activateChildWnd()) activateChildWnd()->cut();		//剪切
}

void MainWindow::docCopy()
{
    if(activateChildWnd()) activateChildWnd()->copy();		//复制
}

void MainWindow::docPaste()
{
    if(activateChildWnd()) activateChildWnd()->paste();		//粘贴
}


void MainWindow::on_actionback_triggered()
{
    docUndo();
}

void MainWindow::on_actionfront_triggered()
{
    docRedo();
}

void MainWindow::on_actioncut_triggered()
{
    docCut();
}

void MainWindow::on_actioncopy_triggered()
{
    docCopy();
}

void MainWindow::on_actionpaste_triggered()
{
    docPaste();
}

void MainWindow::textBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->actionb->isChecked() ? QFont::Bold : QFont::Normal);
    if(activateChildWnd()) activateChildWnd()->setFormatOnSelectedWord(fmt);
}

void MainWindow::textItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->actioni->isChecked());
    if(activateChildWnd()) activateChildWnd()->setFormatOnSelectedWord(fmt);
}

void MainWindow::textUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->actionu->isChecked());
    if(activateChildWnd()) activateChildWnd()->setFormatOnSelectedWord(fmt);
}

void MainWindow::on_actionb_triggered()
{
    textBold();
}

void MainWindow::on_actionu_triggered()
{
    textUnderline();
}

void MainWindow::on_actioni_triggered()
{
    textItalic();
}

void MainWindow::textFamily(const QString &fmly)
{
    QTextCharFormat fmt;
    fmt.setFontFamily(fmly);
    if(activateChildWnd()) activateChildWnd()->setFormatOnSelectedWord(fmt);
}

void MainWindow::textSize(const QString &ps)
{
    qreal pointSize = ps.toFloat();
    if (ps.toFloat() > 0)
    {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        if(activateChildWnd())activateChildWnd()->setFormatOnSelectedWord(fmt);
    }
}

void MainWindow::on_fontComboBox_activated(const QString &arg1)
{
    textFamily(arg1);
}

void MainWindow::on_sizeComboBox_activated(const QString &arg1)
{
    textSize(arg1);
}

void MainWindow::textAlign(QAction *act)
{
    if(activateChildWnd())
    {
        if(act == ui->actionleft)
            activateChildWnd()->setAlignOfDocumentText(1);
        else if(act == ui->actionright)
            activateChildWnd()->setAlignOfDocumentText(3);
        else if(act == ui->actionst)
            activateChildWnd()->setAlignOfDocumentText(4);
    }
}


void MainWindow::on_actionleft_triggered()
{
    textAlign(ui->actionleft);
}

void MainWindow::on_actionright_triggered()
{
    textAlign(ui->actionright);
}

void MainWindow::on_actionst_triggered()
{
    textAlign(ui->actionst);
}

void MainWindow::textColor()
{
    if(activateChildWnd())
    {
        QColor color = QColorDialog::getColor(activateChildWnd()->textColor(), this);
        if(!color.isValid()) return;
        QTextCharFormat fmt;
        fmt.setForeground(color);
        activateChildWnd()->setFormatOnSelectedWord(fmt);
        QPixmap pix(16, 16);
        pix.fill(color);
        ui->actionc->setIcon(pix);
    }
}

void MainWindow::paraStyle(int sidx)
{
    if (activateChildWnd()) activateChildWnd()->setParaStyle(sidx);
}



void MainWindow::on_ComboBox_activated(int index)
{
    paraStyle(index);
}

void MainWindow::on_actionc_triggered()
{
    textColor();
}

void MainWindow::on_actionprint_triggered()
{
    docPrint();
}

void MainWindow::docPrint()
{
    //打印当然实现得有一个打印机
    QPrinter pter(QPrinter::HighResolution);//设置打印机的像素
    //有了打印架当然得有打印对话框
    QPrintDialog *ddlg=new QPrintDialog(&pter,this);
    if(activateChildWnd()){
        ddlg->setOption(QAbstractPrintDialog::PrintSelection,true);
    }
    ddlg->setWindowTitle("打印文档");

    //保存一下当前活动的子窗口
    MyChildWnd *childWnd=activateChildWnd();
    if(ddlg->exec()==QDialog::Accepted){
        childWnd->print(&pter);
    }

    delete ddlg;
}
