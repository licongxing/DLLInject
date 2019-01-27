#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);
    ~ProcessDialog();
    // 填充进程表格
    void fillProcessTab();
    QString getCurPName();
    int getCurPID();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ProcessDialog *ui;
    QString m_pName;
    int m_pID;
};

#endif // PROCESSDIALOG_H
