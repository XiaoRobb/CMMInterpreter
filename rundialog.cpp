#include "rundialog.h"
#include "ui_rundialog.h"
#include <QDesktopWidget>
#include <QDebug>
#include <QInputDialog>
#include "mainwindow.h"

RunDialog::RunDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RunDialog)
{
    ui->setupUi(this);
    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
    move((availableGeometry.width() - width()) / 2,
         (availableGeometry.height() - height()) / 2);
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    ui->plainTextEdit->setReadOnly(true);
}

QPlainTextEdit *RunDialog::getPlaintTextEdit()
{
    return ui->plainTextEdit;
}

RunDialog::~RunDialog()
{
    delete ui;
}

QString RunDialog::getInput()
{
    QString text = QInputDialog::getText(this, tr("Input"), tr("Input the number:"), QLineEdit::Normal);
    qDebug()<<text;
    return text;
}

void RunDialog::closeEvent(QCloseEvent *event)
{
    MainWindow::mainWindow->closeDebugThing();
    event->accept();
}

