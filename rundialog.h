#ifndef RUNDIALOG_H
#define RUNDIALOG_H

#include <QDialog>
#include <QPlainTextEdit>
#include <QString>

namespace Ui {
class RunDialog;
}

class RunDialog : public QDialog
{
    Q_OBJECT

public:

    explicit RunDialog(QWidget *parent = nullptr);
    QPlainTextEdit * getPlaintTextEdit();
    ~RunDialog() override;
    QString getInput();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Ui::RunDialog *ui;
};

#endif // RUNDIALOG_H
