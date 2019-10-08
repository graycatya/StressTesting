#include "connectmanage.h"
#include <QDebug>
#include <iostream>
#include <sstream>
#include <string>
#include <QString>
#include <QTimer>
#include "tcptool.h"
#include "ThreadPool.h"

ConnectManage::ConnectManage(QString ip, quint16 post, ssize_t threadnum, _MODE mode, QThread *parent) : QThread(parent)
{
    m_sIp = ip;
    m_nPost = post;
    m_nNetWorkThreadNum = threadnum;
    m_yMode = mode;
    m_bThread_Exit = false;
    m_bRun_Stop = false;
    m_nTestTime = 5;
    m_sTestData = "Hello World!";
    m_nTimeOut = 3000;
    m_nThread_Activity_Num = 0;
    m_ySumMsg.Event_Client = 0;
    m_ySumMsg.Max_SumTimeOut = "---";
    m_ySumMsg.Min_SumTimeOut = "---";
    m_ySumMsg.Average_SumTimeOut = "---";

    m_ProcessMsg.clear();
    qRegisterMetaType<QMap<int, _NETWORK_STATUS>>("QMap<int, _NETWORK_STATUS>");
    qRegisterMetaType<QVector<int>>("QVector<int>");

    m_pNetWorkThreadPool = new ThreadPool(static_cast<size_t>(m_nNetWorkThreadNum));
    m_pDataProcessThreadPool = nullptr;
    m_pTimer = nullptr;
    m_pThreadTImer = nullptr;
}

ConnectManage::~ConnectManage()
{
    if(m_pNetWorkThreadPool != nullptr)
    {
        delete m_pNetWorkThreadPool;
        m_pNetWorkThreadPool = nullptr;
    }
    if(m_pDataProcessThreadPool != nullptr)
    {
        delete m_pDataProcessThreadPool;
        m_pDataProcessThreadPool = nullptr;
    }
}

void ConnectManage::SetTestData(QString str)
{
    m_sTestData = str;
}

void ConnectManage::SetTestTime(int time)
{
    m_nTestTime = time;
}

void ConnectManage::SetTimeOut(int timeout)
{
    m_nTimeOut = timeout;
}

void ConnectManage::ConnectSignal()
{
    m_yCondition.notify_all();
}

ssize_t ConnectManage::GetThread_Activity_Num() const
{
    return m_nThread_Activity_Num;
}

QMap<int, _NETWORK_STATUS> ConnectManage::GetRocessMsg() const
{
    return m_ProcessMsg;
}

_NETWORK_SUMMSG ConnectManage::GetSumMsg() const
{
    return this->m_ySumMsg;
}

void ConnectManage::SetStop(bool stop)
{
    m_bRun_Stop = stop;
    if(m_bRun_Stop == false)
    {
        if(m_pThreadTImer == nullptr)
        {
            m_pThreadTImer = new QTimer(this);
            connect(m_pThreadTImer, SIGNAL(timeout()), this, SLOT(ThreadPool_Timer()));
            m_pThreadTImer->start(m_nTestTime * 1000);
        }
        m_yStop.notify_all();
    } else {
        if(m_pThreadTImer->isActive() && m_pThreadTImer != nullptr)
        {
            m_pThreadTImer->stop();
            delete m_pThreadTImer;
            m_pThreadTImer = nullptr;
        }
    }
}

void ConnectManage::Exit()
{
    m_bThread_Exit = true;
    m_yStop.notify_all();
    m_yCondition.notify_all();
    m_yAdminCondition.notify_one();
}

void ConnectManage::StartTimer()
{
    if(m_pTimer == nullptr)
    {
        m_pTimer = new QTimer(this);
        connect(m_pTimer, SIGNAL(timeout()), this, SLOT(HandleTimeOut()));
        m_pTimer->start(1000);
    }
}

void ConnectManage::StopTimer()
{
    if(m_pTimer != nullptr && m_pTimer->isActive())
    {
        m_pTimer->stop();
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

void ConnectManage::run()
{
    m_bThread_Exit = false;
    //压力模式选择
    switch (m_yMode)
    {
        case TCP:
        {
            TcpWork_Run();
            break;
        }
        case UDP:
        {
            UdpWork_Run();
            break;
        }
        case HTTP:
        {
            HttpWork_Run();
            break;
        }
    }

    //主线程 - 20190914 目前未设置功能
    do {
        m_yMutexAdmi.lock();
        m_yAdminCondition.wait(&m_yMutexAdmi);
        m_yMutexAdmi.unlock();
        if(m_bThread_Exit)
        {
            return;
        }
        //qDebug() << "m_nThread_Activity_Num: " << m_nThread_Activity_Num;
        /*m_ySumMsg.Event_Client = 0;
        m_ySumMsg.Max_SumTimeOut = "---";
        m_ySumMsg.Min_SumTimeOut = "---";
        m_ySumMsg.Average_SumTimeOut = "---";*/

        if(m_ProcessMsg.size() > 0)
        {
            int Event_Client = 0;
            float Max_SumTimeOut = -1;
            float Min_SumTimeOut = 10000;
            float Average_SumTimeOut = 0;
            for(int i = 0; i < m_ProcessMsg.size(); i++)
            {
                if(m_ProcessMsg[i].ConnectState == "ConnectState Ok")
                {
                    if(m_ySumMsg.Event_Client == 0)
                    {
                        Max_SumTimeOut = m_ProcessMsg[i].Max_ResponseTime.toFloat();
                        Min_SumTimeOut = m_ProcessMsg[i].Min_ResponseTime.toFloat();
                        Average_SumTimeOut = m_ProcessMsg[i].ResponseTime.toFloat();
                    } else {
                        if(Max_SumTimeOut < m_ProcessMsg[i].Max_ResponseTime.toFloat())
                        {
                            Max_SumTimeOut = m_ProcessMsg[i].Max_ResponseTime.toFloat();
                        }
                        if(Min_SumTimeOut > m_ProcessMsg[i].Min_ResponseTime.toFloat())
                        {
                            Min_SumTimeOut = m_ProcessMsg[i].Min_ResponseTime.toFloat();
                        }
                        Average_SumTimeOut += m_ProcessMsg[i].ResponseTime.toFloat();
                    }
                    Event_Client++;
                }
            }
            if(m_ProcessMsg.size() > 0)
            {
                Average_SumTimeOut = Average_SumTimeOut / static_cast<float>(m_ySumMsg.Event_Client);
            }
            m_ySumMsg.Event_Client = Event_Client;
            if(Event_Client != 0)
            {
                m_ySumMsg.Max_SumTimeOut = QString("%1").arg(static_cast<double>(Max_SumTimeOut));
                m_ySumMsg.Min_SumTimeOut = QString("%1").arg(static_cast<double>(Min_SumTimeOut));
                m_ySumMsg.Average_SumTimeOut = QString("%1").arg(QString::number(static_cast<double>(Average_SumTimeOut), 'f', 3));
            } else {
                m_ySumMsg.Max_SumTimeOut = "---";
                m_ySumMsg.Min_SumTimeOut = "---";
                m_ySumMsg.Average_SumTimeOut = "---";
            }
            //m_ySumMsg.Min_SumTimeOut = QString::number(Min_SumTimeOut);
        }

    } while(!m_bThread_Exit);
}

//TCP测试模块
void ConnectManage::TcpWork_Run()
{
    for(int i = 0; i < m_nNetWorkThreadNum; i++)
    {
        //添加任务到线程池
        m_pNetWorkThreadPool->enqueue([i, this]{
            float Max_Time = 0.000;
            float Min_Time = 0.000;
            _NETWORK_STATUS state;
            state.Max_ResponseTime = "-----------";
            state.Min_ResponseTime = "-----------";
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            QString id = QString::fromStdString(oss.str());
            //消息组包
            state.id = i;
            state.ThreadId = id;

            //连接时需要加锁，不加会出现连接卡死
            m_yMutex.lock();
            TcpTool* tcptool = new TcpTool(m_sIp, m_nPost);
            m_yMutex.unlock();
            //所有连接创建好后进入等待
            m_yMutexsleep.lock();
            m_nThread_Activity_Num++;
            //emit TcpConnect_Signal();
            m_yCondition.wait(&m_yMutexsleep);
            m_yMutexsleep.unlock();

            if(m_bThread_Exit == true)
            {
                if(tcptool != nullptr)
                {
                    tcptool->GetTcpSocket()->close();
                    delete tcptool;
                    tcptool = nullptr;
                }
                return;
            }
            //qDebug() << "Thread wait ok: " << i;
            //工作循环
            do {
                if(tcptool->ReConnection(m_nTimeOut))
                {
                    //连接状态
                    state.ConnectState = "ConnectState Ok";

                    //获取端口号
                    state.Post = QString::number(tcptool->GetTcpSocket()->localPort());

                    //发送数据
                    state.Times = QDateTime::currentDateTime();
                    qint64 m_nTimeStart = state.Times.toMSecsSinceEpoch();
                    tcptool->SendData(m_sTestData, m_nTimeOut);
                    state.SendDataLen = QString::number(m_sTestData.size());
                    //判断数据返回
                    if(tcptool->GetTcpSocket()->waitForReadyRead(m_nTimeOut))
                    {
                       qint64 m_nTimeStop = (QDateTime::currentDateTime()).toMSecsSinceEpoch();
                       float time = static_cast<float>((m_nTimeStop - m_nTimeStart)/1000.0);
                       //回复数据时间
                       state.ResponseTime = QString("%1").arg(static_cast<double>(time));

                       if(state.Max_ResponseTime != "-----------")
                       {
                           if(Max_Time < time)
                           {
                               Max_Time = time;
                               state.Max_ResponseTime = QString("%1").arg(static_cast<double>(Max_Time));
                           }
                       }
                       if(state.Min_ResponseTime != "-----------")
                       {
                           if(Min_Time > time)
                           {
                               Min_Time = time;
                               state.Min_ResponseTime = QString("%1").arg(static_cast<double>(Min_Time));
                           }
                       }
                       if(state.Max_ResponseTime == "-----------")
                       {
                           Max_Time = time;
                           state.Max_ResponseTime = QString("%1").arg(static_cast<double>(Max_Time));
                       }
                       if(state.Min_ResponseTime == "-----------")
                       {
                           Min_Time = time;
                           state.Min_ResponseTime = QString("%1").arg(static_cast<double>(Min_Time));
                       }

                       state.DataReturnState = "ReceiveData Ok";
                       state.ReadDataLen = QString::number(tcptool->GetTcpSocket()->readAll().size());
                       //qDebug() << "id: " << id << "   " << tcptool->GetTcpSocket()->readAll();
                    } else {
                        state.ResponseTime = "-----------";
                        state.DataReturnState = "----------";
                    }
                } else {
                    state.ResponseTime = "-----------";
                    state.ConnectState = "-----------";
                    state.DataReturnState = "----------";
                }

                //发送连接信息
                m_yMutex.lock();
                emit Communication_Signal(state);
                m_ProcessMsg[i] = state;
                m_yMutex.unlock();
                //任务时间
                //QThread::sleep(static_cast<unsigned long>(m_nTestTime));

                m_yMutex.lock();
                m_yStop.wait(&m_yMutex);
                m_yMutex.unlock();
                /*
                //停止发送
                if(m_bRun_Stop)
                {
                    qDebug() << "stop";
                    m_yMutex.lock();
                    m_yStop.wait(&m_yMutex);
                    m_yMutex.unlock();
                }*/
                if(m_bThread_Exit)
                {
                    if(tcptool != nullptr)
                    {
                        delete tcptool;
                        tcptool = nullptr;
                    }
                }
            } while(!m_bThread_Exit);

            m_yMutex.lock();
            m_nThread_Activity_Num--;
            m_yMutex.unlock();

        });
    }
}

//UDP测试模块
void ConnectManage::UdpWork_Run()
{
    qDebug() << "This feature is not complete";
    for(int i = 0; i < m_nNetWorkThreadNum; i++)
    {
        m_pNetWorkThreadPool->enqueue([i, this]{
            _NETWORK_STATUS state;
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            QString id = QString::fromStdString(oss.str());

            //消息组包
            state.id = i;
            state.ThreadId = id;
            do {
                //任务时间
                QThread::sleep(static_cast<unsigned long>(m_nTestTime));
            } while(!m_bThread_Exit);
        });
    }
}

//HTTLP测试模块
void ConnectManage::HttpWork_Run()
{
    qDebug() << "This feature is not complete";
    for(int i = 0; i < m_nNetWorkThreadNum; i++)
    {
        m_pNetWorkThreadPool->enqueue([i, this]{
            _NETWORK_STATUS state;
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            QString id = QString::fromStdString(oss.str());

            //消息组包
            state.id = i;
            state.ThreadId = id;
            do {
                //任务时间
                QThread::sleep(static_cast<unsigned long>(m_nTestTime));
            } while(!m_bThread_Exit);
        });
    }
}

void ConnectManage::Communication_Slot(_NETWORK_STATUS state)
{
    /*
    qDebug() << "id: " << state.id << " threadid: " << state.ThreadId << " post: " << state.Post
             << " timeout: " << state.TimeOut << " connectstate: " << state.ConnectState
             << " DataReturnState: " << state.DataReturnState << "DataTime: " << state.Times.toString("yyyy-MM-dd hh:mm:ss");;*/
    qDebug() << "id: " << state.id << " threadid: " << state.ThreadId << " post: " << state.Post << " connectstate: " << state.ConnectState
             << " ResponseTime: " << state.ResponseTime << " Max_ResponseTime: " << state.Max_ResponseTime
             << " Min_ResponseTime: " << state.Min_ResponseTime  << " DataReturnState: " << state.DataReturnState
             << " SendDataLen: " << state.SendDataLen << " ReadDataLen: " << state.ReadDataLen << " DataTime: " << state.Times.toString("yyyy-MM-dd hh:mm:ss");
}

void ConnectManage::HandleTimeOut()
{
    m_yAdminCondition.notify_one();
}

void ConnectManage::ThreadPool_Timer()
{
    m_yStop.notify_all();
}
