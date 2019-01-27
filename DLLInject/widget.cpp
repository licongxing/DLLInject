#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <windows.h>
#include <TlHelp32.h>
#include "workerthread.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),m_procDlg(parent)
{
    ui->setupUi(this);

    m_fileDlg.setWindowTitle(tr("选择注入的DLL"));
    m_fileDlg.setDirectory(".");
    m_fileDlg.setNameFilter(tr("dll文件(*.dll)"));
    m_fileDlg.setViewMode(QFileDialog::Detail);
}

Widget::~Widget()
{
    delete ui;
}

// 选择目标进程
void Widget::on_pushButton_2_clicked()
{
    // 模态展示 进程对话框
    if( m_procDlg.exec() )
    {
        ui->procName->setText(  m_procDlg.getCurPName() );
    }
}

// 选择注入的DLL
void Widget::on_pushButton_clicked()
{
    // 选择DLL文件对话框
    if( m_fileDlg.exec() )
    {
        QStringList list = m_fileDlg.selectedFiles();
        QString dllName = list.first();
        ui->dllPath->setText(dllName);
    }
}

// 注入DLL
void Widget::on_pushButton_3_clicked()
{

    WorkerThread *worker = new WorkerThread(m_procDlg.getCurPID(),ui->dllPath->text(),ENUM_INJECT);
    connect(worker,&WorkerThread::doInjectFinish,this,[=](){
        // 注入dll完毕
        worker->exit();
        delete worker;
    });
    worker->start();
}

// 卸载DLL
void Widget::on_pushButton_4_clicked()
{
    WorkerThread *worker = new WorkerThread(m_procDlg.getCurPID(),ui->dllPath->text(),ENUM_UNINSTALL);
    connect(worker,&WorkerThread::doUninstallFinish,this,[=](){
        // 卸载dll完毕
        worker->exit();
        delete worker;
    });
    worker->start();
}
