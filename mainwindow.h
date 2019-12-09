#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QSessionManager>
#include <QLabel>
#include "mysyntaxhighlighter.h"
#include "rundialog.h"
#include "linebutton.h"
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    void loadFile(const QString &fileName);
    static void writeResult(QString result);
    static QString readInput();
    static void waitForNext();
    static bool isInBreak;
    static QList<int> *breakPoints;
    static int breakAtLineNo;
    static MainWindow * mainWindow;
    void writeValueTable(QString temp);
    void closeDebugThing();
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void run();
    void open();
    bool save();
    void about();
    bool saveAs();
    void aboutUs();
    void newFile();
    void documentWasModified();
    void lexicalAnalysis();
    void syntaxAnalysis();
    void lineLableChange(const QRect &rect, int dy);
    void changeButtonText();
    void debug();
    void next();
    void jixu();
    void valueListShow();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void createButtons(int counts);
    QString strippedName(const QString &fullFileName);
    static void setPointLine(int nowPointLine);
    void sortBreakPoint();
    Ui::MainWindow *ui;
    QString curFile;
    MySyntaxHighlighter *highlier;
    QLabel *lineButtonLabel;
    static QLabel *pointLabel;
    QList<LineButton *> *buttons;
    static RunDialog * resultRunDialog;
    int lineCounts;
    static QElapsedTimer *timer;

};
#endif // MAINWINDOW_H
