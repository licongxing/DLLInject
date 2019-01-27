#include "workerthread.h"
#include <QDebug>
#include <windows.h>
#include <TlHelp32.h>

WorkerThread::WorkerThread(int pid,QString dllPath,WorkFlag flag,QThread *parent) : QThread(parent)
{
    m_pId = pid;
    m_dllPath = dllPath;
    m_flag =flag;
}
void WorkerThread::run()
{
    switch (m_flag) {
        case ENUM_INJECT:
        injectDLL();
        break;
    case ENUM_UNINSTALL:
        uninstallDLL();
        break;
    }
}

// 注入DLL
void WorkerThread::injectDLL()
{
    /*
    注入DLL的思路步骤：
    1. 在目标进程中申请一块内存空间(使用VirtualAllocEx函数) 存放DLL的路径，方便后续执行LoadLibraryA
    2. 将DLL路线写入到目标进程(使用WriteProcessMemory函数)
    3. 获取LoadLibraryA函数地址(使用GetProcAddress)，将其做为线程的回调函数
    4. 在目标进程 创建线程并执行(使用CreateRemoteThread)
    */
    HANDLE targetProc = OpenProcess(PROCESS_ALL_ACCESS,FALSE,m_pId);
    if( targetProc == NULL )
    {
        qDebug() << "OpenProcess error";
        return;
    }
    QString dllPath = m_dllPath;
    const char* pChar = dllPath.toStdString().c_str();
    int dllLen = dllPath.length();
    // 1.目标进程申请空间
    LPVOID pDLLPath = VirtualAllocEx(targetProc,NULL,dllLen,MEM_COMMIT,PAGE_READWRITE );
    if( pDLLPath == NULL )
    {
        qDebug() << "VirtualAllocEx error";
        return;
    }
    SIZE_T wLen = 0;
    // 2.将DLL路径写进目标进程内存空间
    int ret = WriteProcessMemory(targetProc,pDLLPath,pChar,dllLen,&wLen);
    if( ret == 0 )
    {
        qDebug() << "WriteProcessMemory error";
        return;
    }
    // 3.获取LoadLibraryA函数地址
    FARPROC myLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"),"LoadLibraryA");
    if( myLoadLibrary == NULL )
    {
        qDebug() << "GetProcAddress error";
        return;
    }
    // 4.在目标进程执行LoadLibrary 注入指定的线程
    HANDLE tHandle = CreateRemoteThread(targetProc,NULL,NULL,
                       (LPTHREAD_START_ROUTINE)myLoadLibrary,pDLLPath,NULL,NULL);
    if(tHandle == NULL)
    {
        qDebug() << "CreateRemoteThread error";
        return ;
    }
    qDebug() << "注入，wait ..." ;
    WaitForSingleObject(tHandle,INFINITY);
    CloseHandle(tHandle);
    CloseHandle(targetProc);
    qDebug() << "注入，finish ...";
    emit doInjectFinish();
}
// 卸载DLL
void WorkerThread::uninstallDLL()
{
    /*
    卸载步骤和注入DLL步骤实质差不多.
    注入DLL是 在目标进程中执行LoadLibraryA
    卸载DLL是 在目标进程中执行FreeLibrary函数，不同的是卸载不需要再目标进程中申请空间，
    因为FreeLibrary参数为HMODULE 实际上就是一个指针值。这个句柄已经加载就已经存在。
    */
    HANDLE targetProc = OpenProcess(PROCESS_ALL_ACCESS,FALSE,m_pId);
    if( targetProc == NULL )
    {
        qDebug() << "OpenProcess error";
        return;
    }
    QString dllPath = m_dllPath;
    // 1. 获取卸载dll的模块句柄
    HANDLE snapHandele = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE ,m_pId);
    if( INVALID_HANDLE_VALUE == snapHandele)
    {
        qDebug() << "CreateToolhelp32Snapshot error" ;
        return;
    }
    MODULEENTRY32 entry = {0};
    entry.dwSize = sizeof(entry);// 长度必须赋值
    BOOL ret = Module32First(snapHandele,&entry);
    HMODULE dllHandle = NULL;
    while (ret) {
        QString dllName = QString::fromWCharArray(entry.szModule);
        if(dllPath.endsWith(dllName))
        {
            dllHandle = entry.hModule;
            qDebug() << dllName;
            break;
        }
        ret = Module32Next(snapHandele,&entry);
    }
    CloseHandle(snapHandele);
    if( dllHandle == NULL )
    {
        qDebug() << "dll 并未被加载";
        return;
    }

    // 2.获取FreeLibrary函数地址
    FARPROC myLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"),"FreeLibrary");
    if( myLoadLibrary == NULL )
    {
        qDebug() << "GetProcAddress error";
        return;
    }
    // 3.在目标进程执行FreeLibrary 卸载指定的线程
    HANDLE tHandle = CreateRemoteThread(targetProc,NULL,NULL,
                       (LPTHREAD_START_ROUTINE)myLoadLibrary,dllHandle,NULL,NULL);
    if(tHandle == NULL)
    {
        qDebug() << "CreateRemoteThread error";
        return ;
    }
    qDebug() << "卸载，wait ..." ;
    WaitForSingleObject(tHandle,INFINITY);
    CloseHandle(tHandle);
    CloseHandle(targetProc);
    qDebug() << "卸载，finish ...";
    emit doUninstallFinish();
}
