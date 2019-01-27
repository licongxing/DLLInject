#include "processdialog.h"
#include "ui_processdialog.h"
#include <QDebug>
#include <windows.h>
#include <TlHelp32.h>

ProcessDialog::ProcessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);
    fillProcessTab();
    ui->processTab->setColumnWidth(0,200);

}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

// 填充进程表格
void ProcessDialog::fillProcessTab()
{
    // 清空表格
    int rowCount = ui->processTab->rowCount();
    for( int i = 0; i < rowCount; i++ )
    {
        ui->processTab->removeRow(0);
    }

    HANDLE snapHandele = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,NULL);
    if( INVALID_HANDLE_VALUE == snapHandele)
    {
        qDebug() << "CreateToolhelp32Snapshot error" ;
        return;
    }

    PROCESSENTRY32 entry = {0};
    entry.dwSize = sizeof(entry);// 长度必须赋值
    BOOL ret = Process32First(snapHandele,&entry);
    int i = 0;
    while (ret) {
        QString exeFile = QString::fromWCharArray(entry.szExeFile);
        ui->processTab->insertRow(i);
        ui->processTab->setItem(i,0,new QTableWidgetItem(exeFile));
        ui->processTab->setItem(i,1,new QTableWidgetItem(QString("%1").arg(entry.th32ProcessID)));
        i++;
        ret = Process32Next(snapHandele,&entry);
    }
    CloseHandle(snapHandele);
}
QString ProcessDialog::getCurPName()
{
    return m_pName;
}
int ProcessDialog::getCurPID()
{
    return m_pID;
}
// 确定
void ProcessDialog::on_pushButton_clicked()
{
    // 获取选中的进程名和PID
    int row = ui->processTab->currentRow();
    if( row == 0 )
    {
        qDebug() << "请选择注入DLL的目标进程";
        return ;
    }
    m_pName = ui->processTab->item(row,0)->text();
    m_pID = ui->processTab->item(row,1)->text().toInt();
    qDebug() << "process name:" << m_pName << " PID:" << m_pID;
    this->accept();
}

// 刷新进程表格
void ProcessDialog::on_pushButton_2_clicked()
{
    fillProcessTab();
}
