#include "logwidget.h"
#include "ui_logwidget.h"
#include <QFont>
#include <QFile>
#include <QThread>
#include <QTextStream>
#include <QCryptographicHash>
#include <QDebug>
#include <QColor>
#include <QString>
#include <QScrollBar>
#include <math.h>
#include <QRegExpValidator>
#include "Log/CatLog.h"

#define LOG_FOLD_MAX 100
LogWidget::LogWidget(QWidget *parent) : QWidget(parent),
  ui(new Ui::LogWidget)
{
    CatLog::Log::Instance();
    //CATLOG(CatLog::Log::LEVEL_DEBUG, "LogWidget", "...");
    ui->setupUi(this);

    m_bStop = false;
    m_bExit = false;
    m_nIntexLogTest = 1;
    m_nMaxLogTest = 1;
    ConfigUi();
    m_sLogPath = "./Log.log";
    //m_LogStr = new QVector<QString>;
    m_LogStrs.clear();
    m_pThreadPool = new ThreadPool(1);
    connect(this, SIGNAL(LogTimeOut()), this, SLOT(LogTimeOutSlot()));

    m_pThreadPool->enqueue([this]{
        do {
            m_LogStrs.clear();
            m_nLogNum = 0;
            m_nLogErrorNum = 0;
            QFile LogFile(m_sLogPath);
            if (!LogFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                      CATLOG(CatLog::Log::LEVEL_ERROR, "LogWidget file error", "...");
                      continue;
            }
            m_yMd5 = QCryptographicHash::hash(LogFile.readAll(), QCryptographicHash::Md5);
            LogFile.close();
            if (!LogFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                      CATLOG(CatLog::Log::LEVEL_ERROR, "LogWidget file error", "...");
                      continue;
            }
            QTextStream in_(&LogFile);
            while (!in_.atEnd()) {
                in_.readLine();
                m_nLogNum++;
            }
            //qDebug() << "m_nLogNum: " << m_nLogNum;
            LogFile.close();
            if (!LogFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                      CATLOG(CatLog::Log::LEVEL_ERROR, "LogWidget file error", "...");
                      continue;
            }
            QTextStream in(&LogFile);

            int num = 0;
            int intex_fold = 1;
            QVector<QString> temps;
            temps.clear();
            while (!in.atEnd()) {
                QString temp = in.readLine();
                temps.append(temp);

                if(num == LOG_FOLD_MAX - 1)
                {
                    //qDebug() << temps.size();
                    m_LogStrs[intex_fold] = temps;
                    temps.clear();
                    //qDebug() << "num: " << m_LogStrs[intex_fold].size();
                    intex_fold++;
                    num = 0;
                } else {
                    int temp = static_cast<int>(m_nLogNum) - (static_cast<int>(intex_fold-1)*static_cast<int>(LOG_FOLD_MAX));
                    if(temp < LOG_FOLD_MAX && in.atEnd())
                    {
                        //qDebug() << "intex_fold: " << intex_fold;
                        //qDebug() << " end : " << temps.at(temps.size()-1);
                        m_LogStrs[intex_fold] = temps;
                    }
                }
                if(temp.section('|', 2 , 2).trimmed() == "ERROR")
                {
                    m_nLogErrorNum++;
                }
                num++;
            }
            int Paging = static_cast<int>(m_nLogNum)/static_cast<int>(LOG_FOLD_MAX);
            //qDebug() << "m_nLogNum: " << m_nLogNum;
            float Paing_fmod = fmod(static_cast<float>(m_nLogNum),static_cast<float>(1000));
            if(Paing_fmod > 0)
            {
                    Paging++;
            }
            m_nMaxLogTest = Paging;


            emit LogTimeOut();
            QThread::sleep(1);
            if(m_bStop)
            {
                m_Mutex.lock();
                m_WaitCondition.wait(&m_Mutex);
                m_Mutex.unlock();
            }
        } while(!m_bExit);
    });

}

LogWidget::~LogWidget()
{
    if(m_pThreadPool != nullptr)
    {
        m_WaitCondition.notify_all();
        m_bExit = true;
        delete m_pThreadPool;
        m_pThreadPool = nullptr;
    }
}

//2019-09-20 | 09:12:23 | ERROR | Msg: Thread this log LEVEL_ERROR
//设置Log文件路径
void LogWidget::SetLogPath(QString Path)
{
    m_sLogPath = Path;
}

Ui::LogWidget *LogWidget::GetUiWidget()
{
    return ui;
}

//配置LogWidget - 界面属性
void LogWidget::ConfigUi()
{
    ui->LogTextBrowser->setReadOnly(true);
    ui->LogTextBrowser->setStyleSheet("background:rgba(79,80,81,50%);");
    QPalette palette  = ui->LogTextBrowser->verticalScrollBar()->palette();
    palette.setColor(QPalette::Window, Qt::WA_TranslucentBackground);
    ui->LogTextBrowser->verticalScrollBar()->setPalette(palette);

    ui->LogTextBrowser->verticalScrollBar()->setStyleSheet("QScrollBar:vertical"
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
    //WidgetTextControl
    ui->LogNum->setText("------ 日志条目");
    ui->LogNum->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->ErrorNum->setText("ERROR: ------");
    ui->ErrorNum->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->FindEdit->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->LogTest->setText(QString("%1/%2").arg(m_nIntexLogTest).arg(m_nMaxLogTest));
    QRegExp rx("[0-9\\.]+$");
    QRegExpValidator *validator = new QRegExpValidator(rx, this);
    ui->FindEdit->setValidator(validator);
    ui->FindEdit->setText("1");
}

QString LogWidget::LogColor(QString log)
{
    if(log == "ERROR")
        return "#ff0000";
    if(log == "DEBUG")
        return "#5500ff";
    if(log == "WARN")
        return "#ff5500";
    if(log == "INFO")
        return "#aaff7f";
    if(log == "ALARM")
        return "#ffaa00";
    if(log == "FATAL")
        return "#aa0000";
    if(log == "TIME")
        return "#55aa00";
    return "#ffffff";
}

QString LogWidget::LogSliceIn(QString log)
{
    QString Logstr = QString("<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"> \
                             <span style=\" font-weight:600; font-style:italic; color:%1;\">%2</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:#ffffff;\">|</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:%3;\">%4</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:#ffffff;\">|</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:%5;\">%6</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:#ffffff;\">|</span><span style=\" font-weight:600; color:#ffffff;\"> \
                             </span><span style=\" font-weight:600; font-style:italic; color:%7;\">%8</span><span style=\" font-weight:600; color:#ffffff;\"></p>\n")
            .arg(LogColor("TIME")).arg(log.section('|', 0 , 0).trimmed())
            .arg(LogColor("TIME")).arg(log.section('|', 1 , 1).trimmed())
            .arg(LogColor(log.section('|', 2 , 2).trimmed())).arg(log.section('|', 2 , 2).trimmed())
            .arg(LogColor("")).arg(log.section('|', 3 , 3).trimmed());

    return Logstr;
}

void LogWidget::LogTimeOutSlot()
{
    static QByteArray md5 = "----------";
    if(md5 != m_yMd5)
    {
        md5 = m_yMd5;
        ui->LogTextBrowser->clear();
        m_nIntexLogTest = m_nMaxLogTest;
        //qDebug() << "m_LogStrs[m_nMaxLogTest].size(): " << m_LogStrs[m_nMaxLogTest].size();
        for(int i = 0; i < m_LogStrs[m_nMaxLogTest].size(); i++)
        {

            //ui->LogTextBrowser->append(m_LogStr->at(i));
            ui->LogTextBrowser->append(LogSliceIn(m_LogStrs[m_nMaxLogTest].at(i)));
        }

        ui->LogNum->setText(QString("%1 日志条目").arg(m_nLogNum));
        ui->ErrorNum->setText(QString("ERROR: %1").arg(m_nLogErrorNum));
        ui->LogTest->setText(QString("%1/%2").arg(m_nIntexLogTest).arg(m_nMaxLogTest));
    }
}

//日志框自动滚动
void LogWidget::AutoScroll()
{
    QTextCursor cursor = ui->LogTextBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->LogTextBrowser->setTextCursor(cursor);
}


void LogWidget::on_Retreat_clicked()
{
    //ui->Retreat->setEnabled(false);
    m_Mutex_Log.lock();
    static int index = -1;
    if(m_nIntexLogTest > 1)
    {
        m_nIntexLogTest--;
    }
    ui->LogTest->setText(QString("%1/%2").arg(m_nIntexLogTest).arg(m_nMaxLogTest));

    if(index != m_nIntexLogTest)
    {
        index = m_nIntexLogTest;
        ui->LogTextBrowser->clear();
        QVector<QString> temp = m_LogStrs[m_nIntexLogTest];
        //QThread::msleep(200);
        for(int i = 0; i < temp.size(); i++)
        {
            ui->LogTextBrowser->append(LogSliceIn(temp[i]));
        }
        if(ui->LogTextBrowser->toPlainText().size() == 0)
        {
            QThread::msleep(100);
            temp = m_LogStrs[m_nIntexLogTest];
            qDebug() << "temp.size(): " << temp.size() << " m_nIntexLogTest: " << m_nIntexLogTest;
            for(int i = 0; i < temp.size(); i++)
            {
                ui->LogTextBrowser->append(LogSliceIn(temp[i]));
            }
        }
    }
    m_Mutex_Log.unlock();
    //ui->Retreat->setEnabled(true);
}

void LogWidget::on_Advance_clicked()
{
    //ui->Advance->setEnabled(false);
    m_Mutex_Log.lock();
    static int index = -1;

    if(m_nIntexLogTest < m_nMaxLogTest)
    {
        m_nIntexLogTest++;
    }
    ui->LogTest->setText(QString("%1/%2").arg(m_nIntexLogTest).arg(m_nMaxLogTest));

    if(index != m_nIntexLogTest)
    {
        index = m_nIntexLogTest;
        ui->LogTextBrowser->clear();
        QVector<QString> temp = m_LogStrs[m_nIntexLogTest];
        //QThread::msleep(200);
        for(int i = 0; i < temp.size(); i++)
        {
            ui->LogTextBrowser->append(LogSliceIn(temp[i]));
        }
        if(ui->LogTextBrowser->toPlainText().size() == 0)
        {
            QThread::msleep(100);
            temp = m_LogStrs[m_nIntexLogTest];
            //qDebug() << "temp.size(): " << temp.size() << " m_nIntexLogTest: " << m_nIntexLogTest;
            for(int i = 0; i < temp.size(); i++)
            {
                ui->LogTextBrowser->append(LogSliceIn(temp[i]));
            }
        }
    }
    m_Mutex_Log.unlock();
    //ui->Advance->setEnabled(true);
}

void LogWidget::on_Find_clicked()
{
    //ui->Find->setEnabled(false);
    m_Mutex_Log.lock();
    int num = ui->FindEdit->text().toInt();
    if(m_nIntexLogTest != num)
    {
        if(num > 0 && num <= m_nMaxLogTest)
        {
            m_nIntexLogTest = num;
            ui->LogTextBrowser->clear();
            QVector<QString> temp = m_LogStrs[m_nIntexLogTest];
            //QThread::msleep(200);
            for(int i = 0; i < temp.size(); i++)
            {
                ui->LogTextBrowser->append(LogSliceIn(temp[i]));
            }
            if(ui->LogTextBrowser->toPlainText().size() == 0)
            {
                QThread::msleep(100);
                temp = m_LogStrs[m_nIntexLogTest];
                //qDebug() << "temp.size(): " << temp.size();
                for(int i = 0; i < temp.size(); i++)
                {
                    ui->LogTextBrowser->append(LogSliceIn(temp[i]));
                }
            }
            ui->LogTest->setText(QString("%1/%2").arg(m_nIntexLogTest).arg(m_nMaxLogTest));
        }
    }
    m_Mutex_Log.unlock();
    //ui->Find->setEnabled(true);
}
