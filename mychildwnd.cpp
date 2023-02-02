#include "mychildwnd.h"
#include <QFileInfo>
#include <QPrinter>
MyChildWnd::MyChildWnd()
{
    setAttribute(Qt::WA_DeleteOnClose);			//子窗口关闭时销毁该类的对象实例
    beSaved = false;						//初始文档尚未保存
}

void MyChildWnd::newDoc()
{
    //设置窗口编号
    static int wndSeqNum = 1;
    //将当前打开的文档命名为"Word 文档 编号"的形式，编号在使用一次后自增1
    myCurDocPath = tr("Word 文档 %1").arg(wndSeqNum++);
    //设置窗口标题，文档被改动后在其名称后面显示"*"号标识
    setWindowTitle(myCurDocPath + "[*]" + tr(" - MyWord"));
    //文档被改动时发送contentsChanged()信号，执行自定义docBeModified()槽函数
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));
}

void MyChildWnd::docBeModified()
{
    setWindowModified(document()->isModified());	//判断文档内容是否被修改
}

QString MyChildWnd::getCurDocName()
{
    return QFileInfo(myCurDocPath).fileName();
}

bool MyChildWnd::loadDoc(const QString &docName)
{
    if(!docName.isEmpty())
    {
        if(!QFile::exists(docName))return false;
        QFile doc(docName);
        if(!doc.open(QFile::ReadOnly))return false;
        QByteArray text = doc.readAll();
        QTextCodec *text_codec = Qt::codecForHtml(text);
        QString str = text_codec->toUnicode(text);
        if(Qt::mightBeRichText(str))
        {
            this->setHtml(str);
        } else {
            str = QString::fromLocal8Bit(text);
            this->setPlainText(str);
        }
        setCurDoc(docName);
        connect(document(),SIGNAL(contentsChanged()),this,SLOT(docBeModified()));
        return true;
    }
}

void MyChildWnd::setCurDoc(const QString &docName)
{
    myCurDocPath = QFileInfo(docName).canonicalFilePath();
    beSaved = true;						//文档已经被保存过
    document()->setModified(false);				//文档未被改动
    setWindowModified(false);					//窗口不显示被改动标识
    setWindowTitle(getCurDocName() + "[*]");			//设置文档名为子窗口标题
}

bool MyChildWnd::saveDoc()
{
    if(!beSaved) return saveAsDoc();
    else return saveDocOpt(myCurDocPath);
}

bool MyChildWnd::saveAsDoc()
{
    QString docName = QFileDialog::getSaveFileName(this, tr("另存为"), myCurDocPath, tr("HTML 文档 (*.htm *.html);;所有文件 (*.*)"));
    if(docName.isEmpty()) return false;
    else return saveDocOpt(docName);
}

bool MyChildWnd::saveDocOpt(QString docName)
{
    if(!(docName.endsWith(".htm", Qt::CaseInsensitive) || docName.endsWith  (".html", Qt::CaseInsensitive)))
    {
        docName += ".html";				//默认保存为 HTML 文档
    }
    QTextDocumentWriter writer(docName);
    bool success = writer.write(this->document());
    if (success) setCurDoc(docName);
    return success;
}

bool MyChildWnd::promptSave()
{
    if(!document()->isModified()) return true;
    QMessageBox::StandardButton result;
    result = QMessageBox::warning(this, tr("MyWord"), tr("文档'%1'已被更改,保存吗?").arg(getCurDocName()), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if(result == QMessageBox::Save) return saveDoc();
    else if (result == QMessageBox::Cancel) return false;
    return true;
}

void MyChildWnd::closeEvent(QCloseEvent *event)
{
    if(promptSave())
    {
        event->accept();
    } else {
        event->ignore();
    }
}

void MyChildWnd::setFormatOnSelectedWord(const QTextCharFormat &fmt)
{
    QTextCursor tcursor = this->textCursor();
    if(!tcursor.hasSelection()) tcursor.select(QTextCursor::WordUnderCursor);
    tcursor.mergeCharFormat(fmt);
    this->mergeCurrentCharFormat(fmt);
}

void MyChildWnd::setAlignOfDocumentText(int aligntype)
{
    if(aligntype==1)this->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    else if(aligntype==2)this->setAlignment(Qt::AlignHCenter);
    else if(aligntype==3)this->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    else if(aligntype==4) this->setAlignment(Qt::AlignJustify);
}

void MyChildWnd::setParaStyle(int pstyle)
{
    QTextCursor tcursor = this->textCursor();
    if (pstyle != 0)
    {
        QTextListFormat::Style sname = QTextListFormat::ListDisc;
        switch (pstyle)
        {
            default:
            case 1:
                sname = QTextListFormat::ListDisc;		//实心圆标号
                break;
            case 2:
                sname = QTextListFormat::ListCircle;		//空心圆标号
                break;
            case 3:
                sname = QTextListFormat::ListSquare;		//方形标号
                break;
            case 4:
                sname = QTextListFormat::ListDecimal;		//十进制编号
                break;
            case 5:
                sname = QTextListFormat::ListLowerAlpha;	//小写字母编号
                break;
            case 6:
                sname = QTextListFormat::ListUpperAlpha;	//大写字母编号
                break;
            case 7:
                sname = QTextListFormat::ListLowerRoman;	//小写罗马数字编号
                break;
            case 8:
                sname = QTextListFormat::ListUpperRoman;	//大写罗马数字编号
                break;
        }
        tcursor.beginEditBlock();
        QTextBlockFormat tBlockFmt = tcursor.blockFormat();
        QTextListFormat tListFmt;
        if(tcursor.currentList())
        {
            tListFmt = tcursor.currentList()->format();
        } else {
            tListFmt.setIndent(tBlockFmt.indent() + 1);
            tBlockFmt.setIndent(0);
            tcursor.setBlockFormat(tBlockFmt);
        }
        tListFmt.setStyle(sname);
        tcursor.createList(tListFmt);
        tcursor.endEditBlock();
    } else {
        QTextBlockFormat tbfmt;
        tbfmt.setObjectIndex(-1);
        tcursor.mergeBlockFormat(tbfmt);					//合并格式
    }
}

