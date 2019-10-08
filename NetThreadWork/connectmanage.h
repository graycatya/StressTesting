#ifndef CONNECTMANAGE_H
#define CONNECTMANAGE_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QMutex>
#include <QWaitCondition>
#include <QDateTime>


class QTimer;
class TcpTool;
class ThreadPool;

struct _NETWORK_STATUS
{
    int id;                     //任务id - 写表用
    QString ThreadId;           //线程id
    QString Post;               //连接端口号
    QString Max_ResponseTime;   //最大的响应时间
    QString Min_ResponseTime;   //最小的响应时间
    QString ResponseTime;       //服务器处理活动用时
    QString ConnectState;       //连接状态
    QString DataReturnState;    //返回数据是否成功
    QString SendDataLen;        //发送数据长度
    QString ReadDataLen;        //接收数据长度
    QDateTime Times;            //活动时间
};

struct _NETWORK_SUMMSG
{
    int Event_Client;
    QString Max_SumTimeOut;
    QString Min_SumTimeOut;
    QString Average_SumTimeOut;
};

class ConnectManage : public QThread
{
    Q_OBJECT
    Q_ENUMS(_MODE)
public:
    enum _MODE
    {
        TCP,
        UDP,
        HTTP
    };
    explicit ConnectManage(QString ip, quint16 post, ssize_t threadnum, _MODE mode = TCP, QThread *parent = nullptr);
    ~ConnectManage();

    void SetTestData(QString str);
    void SetTestTime(int time);
    void SetTimeOut(int timeout);
    void ConnectSignal( void );
    ssize_t GetThread_Activity_Num( void ) const;
    QMap<int, _NETWORK_STATUS> GetRocessMsg( void ) const;
    _NETWORK_SUMMSG GetSumMsg( void ) const;
    void SetStop(bool stop);
    void Exit( void );

    void StartTimer( void );
    void StopTimer( void );

protected:

private:
    void run();
    void TcpWork_Run( void );
    void UdpWork_Run( void );
    void HttpWork_Run( void );

private:
    QString m_sIp;
    quint16 m_nPost;
    ssize_t m_nNetWorkThreadNum;              //网络任务线程池大小
    ThreadPool* m_pNetWorkThreadPool;         //网络任务线程池
    ThreadPool* m_pDataProcessThreadPool;     //数据处理线程

    QMutex m_yMutex;
    QMutex m_yMutexsleep;
    QMutex m_yMutexAdmi;                      //管理线程-锁
    QWaitCondition m_yCondition;              //子线程阻塞
    QWaitCondition m_yStop;                   //停止发送数据
    QWaitCondition m_yAdminCondition;         //管理线程-条件变量

    _MODE m_yMode;                            //模式选择
    QMap<int, _NETWORK_STATUS> m_ProcessMsg;  //数据存储
    _NETWORK_SUMMSG m_ySumMsg;

    QString m_sTestData;    //测试数据
    int m_nTestTime;        //子线程执行任务时间周期
    int m_nTimeOut;         //连接,发送，接收超时时间

    _NETWORK_STATUS m_yStates;
    bool m_bThread_Exit;            //子线程退出
    bool m_bRun_Stop;               //停止测试
    ssize_t m_nThread_Activity_Num; //当前线程活动数量
    QTimer *m_pTimer;               //定时器
    QTimer *m_pThreadTImer;

signals:
    //void TcpConnect_Signal();
    void Communication_Signal(_NETWORK_STATUS);
    void Concurrent_Information(QMap<int, _NETWORK_STATUS>);

public slots:

private slots:
    //void TcpConnect_Slots();
    void Communication_Slot(_NETWORK_STATUS state);
    void HandleTimeOut();   //超时处理函数
    void ThreadPool_Timer();

};

#endif // CONNECTMANAGE_H
