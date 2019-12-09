#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QList>
#include <QFont>
#include <QPlainTextEdit>

#include "token.h"
#include "lexicalanalysis.h"
#include "syntaxanalysis.h"
#include "parserexception.h"
#include "interpreter.h"
#include "debuginterpreter.h"
#include <QDialog>
#include <QInputDialog>

RunDialog * MainWindow::resultRunDialog = nullptr;
bool MainWindow::isInBreak = false;
QList<int> *MainWindow::breakPoints = new QList<int>();
QElapsedTimer * MainWindow::timer = nullptr;
int MainWindow::breakAtLineNo = 0;
QLabel *MainWindow::pointLabel = nullptr;
MainWindow * MainWindow::mainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_2->hide();

    //连接信号和槽函数
    connect(ui->plainTextEdit->document(), &QTextDocument::contentsChanged, this, &MainWindow::documentWasModified);
    connect(ui->actionnew, &QAction::triggered, this, &MainWindow::newFile);
    connect(ui->actionopen, &QAction::triggered, this, &MainWindow::open);
    connect(ui->actionsave, &QAction::triggered, this, &MainWindow::save);
    connect(ui->actionsave_as, &QAction::triggered, this, &MainWindow::saveAs);
    connect(ui->actionexit, &QAction::triggered, this, &QWidget::close);
    connect(ui->actioncut, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::cut);
    connect(ui->actioncopy, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::copy);
    connect(ui->actionpaste, &QAction::triggered, ui->plainTextEdit, &QPlainTextEdit::paste);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::about);
    connect(ui->actionAbout_Us, &QAction::triggered, this, &MainWindow::aboutUs);
    connect(ui->actionstart, &QAction::triggered, this, &MainWindow::run);
    connect(ui->actionlexical, &QAction::triggered, this, &MainWindow::lexicalAnalysis);
    connect(ui->actionsyntax, &QAction::triggered, this, &MainWindow::syntaxAnalysis);
    connect(ui->actionnext, &QAction::triggered, this, &MainWindow::next);
    connect(ui->actiondebug, &QAction::triggered, this, &MainWindow::debug);
    connect(ui->actioncontinue, &QAction::triggered, this, &MainWindow::jixu);
    connect(ui->actionvalueTable, &QAction::triggered, this, &MainWindow::valueListShow);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif
    readSettings();
    highlier = new MySyntaxHighlighter(ui->plainTextEdit->document());
    ui->statusBar->showMessage(tr("就绪"));
    setCurrentFile(QString());

    buttons = new QList<LineButton *>();
    lineButtonLabel = new QLabel(ui->label);
    lineButtonLabel->setMinimumWidth(LineButton::width);
    lineButtonLabel->adjustSize();
    lineButtonLabel->move(0, 3);
    lineCounts = 0;


    QFont font;
    font.setPixelSize(20);

    pointLabel = new QLabel(ui->label_2);
    pointLabel->adjustSize();
    pointLabel->setFont(font);
    pointLabel->move(0,5);
    mainWindow = this;

    ui->plainTextEdit->setFont(font);
    connect(ui->plainTextEdit, &QPlainTextEdit::updateRequest, this, &MainWindow::lineLableChange);

    ui->dockWidget->hide();

    setUnifiedTitleAndToolBarOnMac(true);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        ui->plainTextEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("打开文件"), tr("."), tr("CMM Files(*.c *.cpp *.txt)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("另存为"), tr("."), tr("CMM Files(*.c *.cpp *.txt)"));
    if(fileName.isEmpty()){
        qDebug()<<"没选文件";
        return false;
    } else{
        qDebug()<<fileName;
        return saveFile(fileName);
    }
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("关于程序"),
            tr("程序名称：CMMInterpreter（CMM语言解释器）\n "
               "支持打开，编辑，修改，CMM语言文件（.c/.cpp/.txt文件)\n"
               "支持解释执行CMM语言编写的代码"));
}

void MainWindow::aboutUs()
{
   QMessageBox::about(this, tr("关于我们"),
            tr("我们是团队：XXX\n"
               "团队成员：周宏俊，周永逸，王文龙，龙波，李星星，衣春藤"));
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("错误"),
                             tr("文件打开失败 %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    ui->plainTextEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("文件加载成功"), 2000);
}

void MainWindow::writeResult(QString result)
{
    QPlainTextEdit * pte = resultRunDialog->getPlaintTextEdit();
    pte->appendPlainText(result);
    resultRunDialog->update();
}

QString MainWindow::readInput()
{
    QPlainTextEdit * pte = resultRunDialog->getPlaintTextEdit();
    QString text = resultRunDialog->getInput();
    pte->appendPlainText("输入："+text);
    return text;
}

void MainWindow::waitForNext()
{
    //前期准备工作
    mainWindow->ui->label_2->show();
    mainWindow->ui->actionnext->setEnabled(true);
    mainWindow->ui->actioncontinue->setEnabled(true);
    setPointLine(breakAtLineNo);
    QString temp = DebugInterpreter::symbolTable->toString();
    mainWindow->writeValueTable(temp);
    isInBreak = true;
    if(timer == nullptr){
        timer = new QElapsedTimer();
    }
    timer->start();
    while(timer->elapsed()<999999 && timer->isValid()){
        QCoreApplication::processEvents();
    }
    delete timer;
    timer = nullptr;
    mainWindow->closeDebugThing();
}

void MainWindow::writeValueTable(QString temp)
{
    ui->plainTextEdit_2->setPlainText(temp);
}

void MainWindow::closeDebugThing()
{
    ui->label_2->hide();
    ui->actionnext->setEnabled(false);
    ui->actioncontinue->setEnabled(false);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("保存"),
                             tr("文件保存失败 %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QGuiApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << ui->plainTextEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QGuiApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("保存成功"), 2000);
    return true;
}

void MainWindow::documentWasModified()
{
    setWindowModified(ui->plainTextEdit->document()->isModified());
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        qDebug()<<"没有初始配置文件";
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    qDebug()<<"保存窗口设置";
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave()
{
    if (!ui->plainTextEdit->document()->isModified())
        return true;
    const int ret
        = QMessageBox::warning(this, tr("保存修改"),
                               tr("文件有未被保存的修改.\n"
                                  "你是否保存修改？"),
                               tr("是"), tr("否"), tr("取消"));
    switch (ret) {
    case 0:
        return save();
    case 2:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    ui->plainTextEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.c";
    setWindowFilePath(shownName);
}

void MainWindow::createButtons(int counts)
{
    lineButtonLabel->resize(lineButtonLabel->size().width(),counts*LineButton::height);
    for(int i = 0; i<buttons->length(); i++){
        delete buttons->at(i);
    }
    buttons->clear();
    for(int i = 1; i<= counts; i++){
        LineButton * button = new LineButton(lineButtonLabel, i);
        buttons->append(button);
        connect(button, &QPushButton::clicked, this, &MainWindow::changeButtonText);
        for(int j=0;j<breakPoints->length();j++){
            if(i == breakPoints->at(j)){
                button->changeText();
                break;
            }
        }
        button->show();
    }
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::setPointLine(int nowPointLine)
{
    QString pointLabelString = "";
    if(nowPointLine != 0){
        for(int i=1; i<nowPointLine; i++){
            pointLabelString += "\n";
        }
        pointLabelString += "=>";
    }
    pointLabel->setText(pointLabelString);
    pointLabel->adjustSize();

}

void MainWindow::sortBreakPoint()
{
        //处理断点，按行号排序（升序）,选择排序
        int len = breakPoints->length();
        for(int i=0;i<len-1;i++){
            for(int j=i+1;j<len;j++){
                if(breakPoints->at(j) > breakPoints->at(i)){
                    //交换
                    breakPoints->swapItemsAt(i,j);
                }
            }
        }
        for(int i=0;i<len;i++){
            qDebug()<<breakPoints->at(i);
        }
}

#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (ui->plainTextEdit->document()->isModified())
            save();
    }
}
#endif

void MainWindow::run()
{
    if(resultRunDialog){
        resultRunDialog->close();
        resultRunDialog = nullptr;
    }
    if(maybeSave()){
        if(ui->plainTextEdit->toPlainText() == "")
            return;
        if(resultRunDialog)
            resultRunDialog->close();
        resultRunDialog = new RunDialog(this);
        resultRunDialog->setModal(false);
        QString showName = curFile;
        if(curFile.isEmpty())
            showName = "untitled.c";
        resultRunDialog->setWindowFilePath(showName);
        QPlainTextEdit * pte = resultRunDialog->getPlaintTextEdit();
        pte->appendPlainText("运行结果：");
        resultRunDialog->show();
        Interpreter::start(ui->plainTextEdit->toPlainText());
    }
}

void MainWindow::lexicalAnalysis()
{
    if(maybeSave()){
        if(ui->plainTextEdit->toPlainText() == "")
            return;
        RunDialog *runDialog = new RunDialog(this);
        runDialog->setModal(false);
        QString showName = curFile;
        if(curFile.isEmpty())
            showName = "untitled.c";
        runDialog->setWindowFilePath(showName);
        QPlainTextEdit * pte = runDialog->getPlaintTextEdit();
        pte->appendPlainText("词法分析程序结果：");
        qDebug()<<ui->plainTextEdit->toPlainText();
        QList<const Token* > * tokens = LexicalAnalysis::lexicalAnalysis(ui->plainTextEdit->toPlainText());
        if(!tokens)
            return;
        for(int i = 0; i < tokens->length(); i++){
            const Token * token = tokens->at(i);
            pte->appendPlainText(token->toStringWithLine());
        }
        runDialog->show();
    }
}

void MainWindow::syntaxAnalysis()
{
    if(maybeSave()){
        if(ui->plainTextEdit->toPlainText() == "")
            return;
        RunDialog *runDialog = new RunDialog(this);
        runDialog->setModal(false);
        QString showName = curFile;
        if(curFile.isEmpty())
            showName = "untitled.c";
        runDialog->setWindowFilePath(showName);
        QPlainTextEdit * pte = runDialog->getPlaintTextEdit();
        pte->appendPlainText("语法分析程序结果：");
        QList<const Token* > * tokens = LexicalAnalysis::lexicalAnalysis(ui->plainTextEdit->toPlainText());
        try {
            QVector<TreeNode *> *nodes = SyntaxAnalysis::syntaxAnalysis(tokens);
            for(int i=0; i< nodes->length(); i++){
                const TreeNode * node = nodes->at(i);
                qDebug()<<node->toString();
                pte->appendPlainText(node->toString());
            }
            pte->appendPlainText("语法正确！！！");
        } catch (ParserException *e) {
            qDebug()<<e->getMessage();
            pte->appendPlainText(e->getMessage());
        }
        runDialog->show();
    }
}


void MainWindow::lineLableChange(const QRect &rect, int dy)
{
    Q_UNUSED(rect)
    int lineCount = ui->plainTextEdit->document()->lineCount();
    if(lineCounts != lineCount){
        if(lineCounts == lineCount+1){
            int lineNo = ui->plainTextEdit->textCursor().blockNumber()+1;
            for(int i=0;i<breakPoints->length();i++){
                if(breakPoints->at(i) == lineNo+1){
                    breakPoints->removeAt(i);
                    break;
                }
            }
        }
        lineCounts = lineCount;
        createButtons(lineCount);
    }
    lineButtonLabel->move(lineButtonLabel->x(), lineButtonLabel->y() + dy);
    pointLabel->move(pointLabel->x(), pointLabel->y() + dy);
}

void MainWindow::changeButtonText()
{
    LineButton * button = dynamic_cast<LineButton *>(sender());


    if(!button->isClicked()){
        breakPoints->append(button->line);
        button->changeText();
        return;
    }
    for(int i=0; i<breakPoints->length(); i++){
        if(breakPoints->at(i) == button->line){
            breakPoints->removeAt(i);
            break;
        }
    }
    button->changeText();
}

void MainWindow::debug()
{
    qDebug()<<"xxxx";
    if(maybeSave()){
        if(ui->plainTextEdit->toPlainText() == "")
            return;
        breakAtLineNo = 0;
        isInBreak = false;
        ui->plainTextEdit_2->clear();
        ui->dockWidget->show();
        ui->actionvalueTable->setEnabled(true);
        if(resultRunDialog){
            resultRunDialog->close();
            resultRunDialog = nullptr;
        }
        if(ui->plainTextEdit->toPlainText() == "")
            return;
        if(resultRunDialog)
            resultRunDialog->close();
        resultRunDialog = new RunDialog(this);
        resultRunDialog->setModal(false);
        QString showName = curFile;
        if(curFile.isEmpty())
            showName = "untitled.c";
        resultRunDialog->setWindowFilePath(showName);
        QPlainTextEdit * pte = resultRunDialog->getPlaintTextEdit();
        pte->appendPlainText("运行结果：");
        resultRunDialog->show();
        DebugInterpreter::start(ui->plainTextEdit->toPlainText());
    }
}

void MainWindow::next()
{
    timer->invalidate();
}

void MainWindow::jixu()
{
    isInBreak = false;
    timer->invalidate();
}

void MainWindow::valueListShow()
{
    ui->dockWidget->show();
}
