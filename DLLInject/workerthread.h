#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H

#include <QThread>
enum WorkFlag{
    ENUM_INJECT,
    ENUM_UNINSTALL
};

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkerThread(int pid,QString dllPath,WorkFlag flag,QThread *parent = nullptr);
    void run();
signals:
    void doInjectFinish();
    void doUninstallFinish();

private:
    // 注入DLL
    void injectDLL();
    // 卸载DLL
    void uninstallDLL();
public slots:

private:
    int m_pId;
    QString m_dllPath;
    WorkFlag m_flag;
};

#endif // WORKERTHREAD_H
