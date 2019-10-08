#include "networkwidget.h"
#include "ui_networkwidget.h"
#include "ui_settingwidget.h"
#include "ui_mainwidget.h"
#include <QDebug>
#include "CustomControl/progressarc.h"
#include "NetThreadWork/connectmanage.h"
#include "NetThreadWork/ThreadPool.h"
#include <QTimer>
#include <QScrollBar>

NetWorkWidget::NetWorkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetWorkWidget)
{
    ui->setupUi(this);
    m_pProgressArc = nullptr;
    m_pThreadPool = nullptr;
    m_pBinButton = nullptr;
    m_pTestButton = nullptr;
    m_bThreadPoolStart = false;
    m_bTestStart = false;
    ui_setting = nullptr;
    //m_pThreadPool = new ThreadPool(2);
    m_pTimer = new QTimer(this);
    m_pTimerNetData = new QTimer(this);
    m_pTimerNetTest = new QTimer(this);

    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(Set_Progress()));
    connect(m_pTimerNetData, SIGNAL(timeout()), this, SLOT(Update_Table()));
    connect(m_pTimerNetTest, SIGNAL(timeout()), this, SLOT(Upate_TestTime()));
    InitWidget();
}

NetWorkWidget::~NetWorkWidget()
{
    if(m_pProgressArc != nullptr)
    {
        delete m_pProgressArc;
        m_pProgressArc = nullptr;
    }
    if(m_pThreadPool != nullptr)
    {
        delete m_pThreadPool;
        m_pThreadPool = nullptr;
    }
    if(m_pTimer != nullptr)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    }
    if(m_pTimerNetData != nullptr)
    {
        delete m_pTimerNetData;
        m_pTimerNetData = nullptr;
    }
    if(m_pTimerNetTest != nullptr)
    {
        delete m_pTimerNetTest;
        m_pTimerNetTest = nullptr;
    }
    delete ui;
}

Ui::NetWorkWidget *NetWorkWidget::GetUiWidget()
{
    return ui;
}

void NetWorkWidget::SetUiSetting(Ui::SettingUpWidget *ui)
{
    ui_setting = ui;
}

void NetWorkWidget::SetBinButton(SwitchButton *button)
{
    m_pBinButton = button;
}

void NetWorkWidget::SetTestButton(SwitchButton *button)
{
    m_pTestButton = button;
}

void NetWorkWidget::SetUiMain(Ui::mainwindow *ui)
{
    ui_main = ui;
}

void NetWorkWidget::SetTableList( QStringList list )
{
    ui->TesttableWidget->setColumnCount(list.size());
    ui->TesttableWidget->setHorizontalHeaderLabels(list);
}

void NetWorkWidget::SetTableLine( int num )
{
    ui->TesttableWidget->setRowCount(num);
}

void NetWorkWidget::SetTableData(int row, int column, QString content)
{
    QTableWidgetItem *item = new QTableWidgetItem(content);
    ui->TesttableWidget->setItem(row, column, item);
}

void NetWorkWidget::InitWidget()
{
    QHBoxLayout *hboxlayout = new QHBoxLayout(ui->LinkingNumwidget);
    m_pProgressArc = new ProgressArc(ui->LinkingNumwidget);
    hboxlayout->addWidget(m_pProgressArc);
    m_pProgressArc->setGeometry(0,0,95,95);
    m_pProgressArc->setMinimumWidth(90);
    m_pProgressArc->setMinimumHeight(90);
    m_pProgressArc->SetBorderWidth(5);
    m_pProgressArc->SetLineWidth(10);
    m_pProgressArc->SetLineColor(QColor(Qt::white));
    m_pProgressArc->SetBorderColor(QColor(Qt::white));
    m_pProgressArc->SetBgColor(QColor(Qt::gray));
    m_pProgressArc->SetValue(0);
    ui->ThreadPoolButton->setToolTip("启动连接池");
    ui->StartButton->setToolTip("启动压力测试");
    ui->StartButton->setEnabled(false);
    //ui->TesttableWidget->horizontalHeader()->setVisible(false);
    //去掉默认序号
    ui->TesttableWidget->verticalHeader()->setVisible(false);
    //设置不可编辑
    ui->TesttableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->TesttableWidget->setFrameShape(QFrame::NoFrame);
    ui->TesttableWidget->setFocusPolicy(Qt::NoFocus);
    ui->TesttableWidget->setShowGrid(false);
    ui->TesttableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //自适应列宽
    ui->TesttableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //设置充满表宽度
    ui->TesttableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->TesttableWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
                                                            "{"
                                                                //设置背景-及滚动条宽度
                                                                "border: 1px solid gray;"
                                                                "width:10px;"
                                                                "background: LightGreen;"
                                                                "margin:0px,0px,0px,0px;"
                                                                //"padding-top:12px;"
                                                                //"padding-bottom:12px;"
                                                            "}"
                                                            "QScrollBar::handle:vertical"
                                                            "{"
                                                                //设置滚动条-手柄颜色即高度
                                                                "width:8px;"
                                                                "background:rgba(79,80,81,50%);"
                                                                "border-radius:8px;"
                                                                "min-height:20;"
                                                            "}"
                                                            "QScrollBar::handle:vertical:hover"
                                                            "{"
                                                                //鼠标触及滑块样式
                                                                "width:8px;"
                                                                "background:rgba(10,10,10,50%);"
                                                                "border-radius:8px;"
                                                                "min-height:20;"
                                                            "}");
    ui->TesttableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar"
                                                           "{"
                                                               //设置背景-及滚动条宽度
                                                               "border: 1px solid gray;"
                                                               "height:10px;"
                                                               "background: LightGreen;"
                                                               "margin:0px,0px,0px,0px;"
                                                               //"padding-top:12px;"
                                                               //"padding-bottom:12px;"
                                                           "}"
                                                           "QScrollBar::handle"
                                                           "{"
                                                               //设置滚动条-手柄颜色即高度
                                                               "height:8px;"
                                                               "background:rgba(79,80,81,50%);"
                                                               "border-radius:8px;"
                                                               "min-height:20;"
                                                           "}"
                                                           "QScrollBar::handle:hover"
                                                           "{"
                                                               //鼠标触及滑块样式
                                                               "height:8px;"
                                                               "background:rgba(10,10,10,50%);"
                                                               "border-radius:8px;"
                                                               "min-height:20;"
                                                           "}");
    ui->TesttableWidget->setStyleSheet("QTableView"
                                        "{"
                                            "background-color: rgba(255, 255, 255, 0);"
                                            "alternate-background-color:#e3edf9;"
                                            "font:14px \"微软雅黑\";"
                                            "color:#368666;"
                                            "gridline-color: #ccddf0;"
                                        "}"
                                       "QTableWidget::item::selected"
                                       "{"
                                       "color:#FFFFFF;"
                                       "background-color:rgb(23,68,100);"
                                       "}");
    ui->TesttableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section"
                                                           "{"
                                                           "color: #BBFFFF;"
                                                           "font:bold 14px \"微软雅黑\";"
                                                           "text-align:center;"
                                                           "height:32px;"
                                                           "background-color: rgba(60,60,60,255);"
                                                           "border:1px solid #8faac9;"
                                                           "border-left:none;"
                                                       "}");
    /*struct _NETWORK_STATUS
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
};*/
    QStringList Headlist;
    Headlist<<"Id"<<"ThreadId"<<"Post"<<"最大的响应时间"<<"最小的响应时间"<<"服务器处理活动用时"<<"连接状态"<<"返回数据校验"<<"发送数据长度"<<"接收数据长度"<<"活动时间";
    SetTableList(Headlist);
    //ui->TesttableWidget->setRowCount(2);
    //SetTableData(1,0, "zzp");

}

void NetWorkWidget::Set_Progress()
{
    int num = static_cast<int>(static_cast<float>(m_pConnectManage->GetThread_Activity_Num()) / static_cast<float>(ui_setting->ThreadNumlineEdit->text().toInt()) * static_cast<float>(100));
    ui_main->ProgressBar->setValue(num);
    ui_main->Hintlabel->setText("创建连接池: " +  QString::number(num) + "%");
    if(static_cast<int>(m_pConnectManage->GetThread_Activity_Num()) == ui_setting->ThreadNumlineEdit->text().toInt())
    {
        m_pTimer->stop();
        ui->ThreadPoolButton->setEnabled(true);
        ui->StartButton->setEnabled(true);
        ui_main->Hintlabel->setText("连接池创建成功");

    }
}

void NetWorkWidget::Update_Table()
{
    //this->m_qWaitCondition.notify_one();

    m_pProgressArc->SetValue(static_cast<int>(static_cast<float>(m_pConnectManage->GetSumMsg().Event_Client) /
                                              static_cast<float>(ui_setting->ThreadNumlineEdit->text().toInt()) * static_cast<float>(100)));
    //qDebug() << "m_pConnectManage->GetSumMsg().Max_SumTimeOut" << m_pConnectManage->GetSumMsg().Max_SumTimeOut;
    //qDebug() << "m_pConnectManage->GetSumMsg().Min_SumTimeOut" << m_pConnectManage->GetSumMsg().Min_SumTimeOut;
    //qDebug() << "m_pConnectManage->GetSumMsg().Average_SumTimeOut" << m_pConnectManage->GetSumMsg().Average_SumTimeOut;
    ui->MaxTimeOutText->setText(QString("%1/ms").arg(m_pConnectManage->GetSumMsg().Max_SumTimeOut));
    ui->MinTimeOutText->setText(QString("%1/ms").arg(m_pConnectManage->GetSumMsg().Min_SumTimeOut));
    ui->AverageTimeOutText->setText(QString("%1/ms").arg(m_pConnectManage->GetSumMsg().Average_SumTimeOut));
}

void NetWorkWidget::Read_NetData(_NETWORK_STATUS data)
{
    SetTableData(data.id, 0, QString::number(data.id));
    SetTableData(data.id, 1, data.ThreadId);
    SetTableData(data.id, 2, data.Post);
    SetTableData(data.id, 3, data.Max_ResponseTime);
    SetTableData(data.id, 4, data.Min_ResponseTime);
    SetTableData(data.id, 5, data.ResponseTime);
    SetTableData(data.id, 6, data.ConnectState);
    SetTableData(data.id, 7, data.DataReturnState);
    SetTableData(data.id, 8, data.SendDataLen);
    SetTableData(data.id, 9, data.ReadDataLen);
    SetTableData(data.id, 10, data.Times.toString("yyyy-MM-dd hh:mm:ss"));
    for(int i = 0; i < 11; i++)
    {
        ui->TesttableWidget->item(data.id,i)->setTextAlignment(Qt::AlignCenter);
    }
}

void NetWorkWidget::Upate_TestTime()
{
    static int time = 0;
    time++;
    int value = static_cast<int>(static_cast<float>(time) / static_cast<float>(ui_setting->TestTimelineEdit->text().toInt() * 60) * 100);
    ui_main->ProgressBar->setValue(value);
    ui_main->Hintlabel->setText("测试进度: " +  QString::number(value) + "%");
    if(time == ui_setting->TestTimelineEdit->text().toInt() * 60)
    {
        time = 0;
        ui->StartButton->click();
        ui_main->Hintlabel->setText("测试完成");

    }
}



void NetWorkWidget::on_ThreadPoolButton_clicked()
{
    m_bThreadPoolStart = !m_bThreadPoolStart;
    if(m_bThreadPoolStart == true)
    {
        ui->ThreadPoolButton->setEnabled(false);
        ui_main->SettiongButton->setEnabled(false);
        ui_main->ProgressBar->setVisible(true);

        m_pConnectManage = new ConnectManage(ui_setting->IplineEdit->text(),
                                             static_cast<quint16>(ui_setting->PostlineEdit->text().toInt()),
                                             ui_setting->ThreadNumlineEdit->text().toInt(),
                                             static_cast<ConnectManage::_MODE>(ui_setting->comboBox->currentIndex()));
        m_pConnectManage->SetTimeOut(ui_setting->TimeOutlineEdit->text().toInt() * 1000);
        if(m_pTestButton->GetChecked())
        {
            m_pConnectManage->SetTestData(ui_setting->SendplainTextEdit->toPlainText());
        }


        connect(m_pConnectManage, SIGNAL(Communication_Signal(_NETWORK_STATUS)), this, SLOT(Read_NetData(_NETWORK_STATUS)), Qt::DirectConnection);
        m_pTimer->start(500);

        m_pConnectManage->start();
        m_pConnectManage->StartTimer();
        SetTableLine(ui_setting->ThreadNumlineEdit->text().toInt());

        m_pTimerNetData->start(500);

        //活动客户端数量 - 最大最小响应时间数据分析
        /*
        m_pThreadPool->enqueue([this]{
            do{
                this->m_qMutex.lock();
                this->m_qWaitCondition.wait(&this->m_qMutex);
                this->m_qMutex.unlock();
                QMap<int, _NETWORK_STATUS> msg = m_pConnectManage->GetRocessMsg();
                for(int i = 0; i < msg.size(); i++)
                {

                }
            }while(m_bThreadPoolStart);
        });*/

    } else {
        m_pProgressArc->SetValue(0);
        ui->MaxTimeOutText->setText(QString("---/ms"));
        ui->MinTimeOutText->setText(QString("---/ms"));
        ui->AverageTimeOutText->setText(QString("---/ms"));
        //ui->TesttableWidget->clear();
        SetTableLine(0);
        ui_main->Hintlabel->setText("压力测试");
        if(m_pTimerNetData->isActive())
        {
            m_pTimerNetData->stop();
        }
        if(m_pConnectManage != nullptr)
        {
            ui_main->ProgressBar->setVisible(false);
            ui_main->SettiongButton->setEnabled(true);
            ui_main->ProgressBar->setValue(0);
            m_pConnectManage->StopTimer();
            m_pConnectManage->Exit();

            m_pConnectManage->wait();
            delete m_pConnectManage;
            m_pConnectManage = nullptr;
        }
        ui->StartButton->setEnabled(false);
    }
}

void NetWorkWidget::on_StartButton_clicked()
{
    m_bTestStart = !m_bTestStart;
    if(m_bTestStart == true)
    {
        m_pConnectManage->ConnectSignal();
        m_pConnectManage->SetStop(false);
        ui->ThreadPoolButton->setEnabled(false);
        m_pTimerNetTest->start(1000);
    } else {
        if(m_pTimerNetTest->isActive())
        {
            m_pTimerNetTest->stop();
        }
        m_pConnectManage->SetStop(true);
        ui->ThreadPoolButton->setEnabled(true);
    }
}
