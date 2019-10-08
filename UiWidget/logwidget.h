#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include "NetThreadWork/ThreadPool.h"
#include <QMutex>
#include <QWaitCondition>
#include <QVector>
#include <QMap>

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(ERROR_LOG)
public:
    enum ERROR_LOG
    {
        FILE_PATH_ERROR
    };
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();
    void SetLogPath(QString Path);
    Ui::LogWidget* GetUiWidget( void );

protected:

private:
    void ConfigUi( void );
    QString LogColor( QString log );
    QString LogSliceIn( QString log );

private:
    Ui::LogWidget *ui;
    ThreadPool* m_pThreadPool;
    QString m_sLogPath;
    //QVector<QString> m_LogStr;
    QMap<int, QVector<QString>> m_LogStrs;
    QByteArray m_yMd5;

    int m_nIntexLogTest;
    int m_nMaxLogTest;

    bool m_bWorkingCondition;       //工作状态 - 判断日志可视化模块工作是否正常
    bool m_bStop;                   //用来启动和停止日志显示模块
    bool m_bExit;                   //日式模块退出

    int64_t m_nLogNum;              //日志条目数量
    int64_t m_nLogErrorNum;         //ERROR等级日志数量

    QMutex m_Mutex_Log;
    QMutex m_Mutex;
    QWaitCondition m_WaitCondition;

signals:
    void LogTimeOut();

public slots:
    void LogTimeOutSlot();
    void AutoScroll();      //自动滚动
private slots:

    void on_Retreat_clicked();
    void on_Advance_clicked();
    void on_Find_clicked();
};

#endif // LOGWIDGET_H
