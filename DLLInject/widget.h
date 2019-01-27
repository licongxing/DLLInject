#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "processdialog.h"
#include <QFileDialog>
#include <QThread>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
private:
    QString getCurPName();
    int getCurPID();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::Widget *ui;
    ProcessDialog m_procDlg;
    QFileDialog m_fileDlg;
    QThread m_thread;
};

#endif // WIDGET_H
