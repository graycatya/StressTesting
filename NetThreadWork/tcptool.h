#ifndef TCPTOOL_H
#define TCPTOOL_H

#include <QTcpSocket>
#include <QMap>
#include "ThreadPool.h"
#include <mutex>

class TcpTool
{
public:
    explicit TcpTool(QString ip, quint16 post);
    ~TcpTool();

    bool SendData(QString data, int timeout);
    QByteArray ReadData( int timeout );
    bool ReConnection( int timeout );
    QTcpSocket* GetTcpSocket( void ) const;

protected:


private:
    QString m_sIp;
    quint16 m_nPost;
    QTcpSocket* m_ysocket;

};

#endif // TCPTOOL_H
