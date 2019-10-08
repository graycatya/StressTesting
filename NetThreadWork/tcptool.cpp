#include "tcptool.h"
#include <QDebug>
#include <QThread>
#include <mutex>

TcpTool::TcpTool(QString ip, quint16 post)
{
    m_sIp = ip;
    m_nPost = post;
    m_ysocket = new QTcpSocket;

    m_ysocket->connectToHost(m_sIp, m_nPost);
}

TcpTool::~TcpTool()
{
    if(m_ysocket != nullptr)
    {
        m_ysocket->close();
        delete m_ysocket;
        m_ysocket = nullptr;
    }
}

bool TcpTool::SendData(QString data, int timeout)
{
    m_ysocket->write(data.toUtf8());
    return m_ysocket->waitForBytesWritten(timeout);
}

QByteArray TcpTool::ReadData(int timeout)
{
    QByteArray str = "ERROR!";
    if(m_ysocket->waitForReadyRead(timeout))
    {
        return m_ysocket->readAll();
    }
    return str;
}


bool TcpTool::ReConnection(int timeout)
{

    if(m_ysocket->waitForConnected(timeout))
    {
        return true;
    } else {
        m_ysocket->connectToHost(m_sIp, m_nPost);
    }

    return false;
}

QTcpSocket *TcpTool::GetTcpSocket() const
{
    return m_ysocket;
}

