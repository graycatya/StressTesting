#ifndef NETWORKWIDGET_H
#define NETWORKWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QWaitCondition>
#include <NetThreadWork/connectmanage.h>
#include "CustomControl/switchbutton.h"

namespace Ui {
    class NetWorkWidget;
}

namespace Ui {
    class mainwindow;
}

namespace Ui{
    class SettingUpWidget;
}

//struct _NETWORK_STATUS;

class ProgressArc;
class ConnectManage;
class ThreadPool;
class QTimer;

class NetWorkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetWorkWidget(QWidget *parent = nullptr);
    ~NetWorkWidget();
    Ui::NetWorkWidget* GetUiWidget( void );
    void SetUiSetting(Ui::SettingUpWidget *ui);
    void SetBinButton( SwitchButton* button );
    void SetTestButton( SwitchButton* button );
    void SetUiMain(Ui::mainwindow *ui);
    void SetTableList( QStringList list );
    void SetTableLine( int num );
    void SetTableData(int row, int column, QString content);


private slots:
    void on_ThreadPoolButton_clicked();

    void on_StartButton_clicked();

private:
   void InitWidget( void );

private:
    Ui::NetWorkWidget *ui;
    Ui::mainwindow* ui_main;
    Ui::SettingUpWidget* ui_setting;

    SwitchButton* m_pBinButton;
    SwitchButton* m_pTestButton;

    ThreadPool *m_pThreadPool;
    ProgressArc *m_pProgressArc;
    ConnectManage *m_pConnectManage;

    QMutex m_qMutex;
    QWaitCondition m_qWaitCondition;
    QTimer *m_pTimer;
    QTimer *m_pTimerNetData;
    QTimer *m_pTimerNetTest;
    bool m_bThreadPoolStart;
    bool m_bTestStart;

signals:
    void Send_Progress(int);

private slots:
    void Set_Progress();
    void Update_Table();
    void Read_NetData(_NETWORK_STATUS data);
    void Upate_TestTime();
};

#endif // NETWORKWIDGET_H
