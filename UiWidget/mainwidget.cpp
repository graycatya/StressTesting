#include "mainwidget.h"
#include "ui_mainwidget.h"

#include <QPen>
#include <QDebug>
#include <QMouseEvent>
#include <QThread>
#include <QToolButton>
#include <QDesktopServices>
#include <QVBoxLayout>

#include "Log/CatLog.h"
#include "networkwidget.h"
#include "logwidget.h"
#include "settingwidget.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainwindow)
{
    ui->setupUi(this);
    InitWidget();
    m_bPressed = false;
    m_bPosition = false;
    ui_net = nullptr;
    //m_pThreadPool = new ThreadPool(1);

    connect(ui->MinmizeButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(ui->CloseButtion, SIGNAL(clicked()), this, SLOT(close()));
    /*
    connect(this, SIGNAL(textsignals()), this, SLOT(textslots()));
    m_pThreadPool->enqueue([this]{
        while(1)
        {
            emit textsignals();
            QThread::msleep(100);
        }
    });*/
}

MainWidget::~MainWidget()
{
    delete ui;
    if(m_pThreadPool != nullptr)
    {
        delete m_pThreadPool;
        m_pThreadPool = nullptr;
    }
    if(logwidget != nullptr)
    {
        delete logwidget;
        logwidget = nullptr;
    }
    CatLog::Log::Delete();
}

Ui::mainwindow* MainWidget::GetUiWidget()
{
    return ui;
}

void MainWidget::SetUiNetWork(Ui::NetWorkWidget *ui)
{
    ui_net = ui;
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    m_yWidgetPoint = this->pos();
    m_yMousePoint = event->globalPos();
    m_yNewPoint = m_yMousePoint - m_yWidgetPoint;
    m_bPressed = true;
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!m_bPressed)
    {
        PositionEvent(event);
    }

    if(m_bPosition == true && m_bPressed == true)
    {
        ShowWidget(m_yPosition, event);
    }

    if(this->m_yNewPoint.ry() < ui->HeadWidget->height() &&
            this->m_yNewPoint.rx() < ui->MinmizeButton->pos().rx() &&
            m_bPressed == true && m_bPosition == false && m_yPosition == DRAG_KEEP &&
            this->m_yNewPoint.ry() > 1 && this->m_yNewPoint.rx() > 1)
    {
        this->move(event->globalPos() - this->m_yNewPoint);
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_bPressed = false;
}

MainWidget::DRAG_ABSOLUTE_POSITION MainWidget::MousePosition(QPoint point)
{
    #define  DRAG_THE_SPACING 1
    #define  REMOVE_THE_VALUE 8

    if((point.rx() <= DRAG_THE_SPACING && point.rx() >= 0) &&
            !(point.ry() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.ry() >= 0) &&
            !(point.ry() >= (this->height() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.ry() <= this->height()))
    {
        return DRAG_LOCK_LEFT;
    }

    if(( (point.ry() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.ry() >= 0) && (point.rx() <= DRAG_THE_SPACING && point.rx() >= 0) )
            || ( (point.rx() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.rx() >= 0) && (point.ry() <= DRAG_THE_SPACING && point.ry() >= 0)  ))
    {
        return DRAG_LOCK_LEFTTOP;
    }

    if(( (point.rx() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.rx() >= 0) && (point.ry() >= (this->height() - DRAG_THE_SPACING) && point.ry() <= this->height()) )
            || ( (point.ry() >= (this->height() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.ry() <= this->height()) && (point.rx() <= DRAG_THE_SPACING && point.rx() >= 0) ))
    {
        return DRAG_LOCK_LEFTBOTTOM;
    }

    if((point.rx() >= (this->width() - DRAG_THE_SPACING) && point.rx() <= this->width()) &&
            !(point.ry() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.ry() >= 0) &&
            !(point.ry() >= (this->height() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.ry() <= this->height()))
    {
        return DRAG_LOCK_RIGHT;
    }

    if(( (point.rx() >= (this->width() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.rx() <= this->width()) && (point.ry() <= DRAG_THE_SPACING && point.ry() >= 0))
            || ( (point.ry() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.ry() >= 0) && (point.rx() >= (this->width() - DRAG_THE_SPACING) && point.rx() <= this->width())))
    {
        return DRAG_LOCK_RIGHTTOP;
    }

    if(( (point.rx() >= (this->width() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.rx() <= this->width()) && (point.ry() >= (this->height() - DRAG_THE_SPACING) && point.ry() <= this->height()))
            || ( (point.ry() >= (this->height() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.ry() <= this->height()) && (point.rx() >= (this->width() - DRAG_THE_SPACING) && point.rx() <= this->width()) ))
    {
        return DRAG_LOCK_RIGHTBOTTOM;
    }

    if((point.ry() <= DRAG_THE_SPACING && point.ry() >= 0) &&
            !(point.rx() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.rx() >= 0) &&
            !(point.rx() >= (this->width() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.rx() <= this->width()))
    {
        return DRAG_LOCK_TOP;
    }

    if((point.ry() >= (this->height() - DRAG_THE_SPACING) && point.ry() <= this->height()) &&
            !(point.rx() <= DRAG_THE_SPACING + REMOVE_THE_VALUE && point.rx() >= 0) &&
            !(point.rx() >= (this->width() - DRAG_THE_SPACING - REMOVE_THE_VALUE) && point.rx() <= this->width()))
    {
        return DRAG_LOCK_BOTTOM;
    }

    return DRAG_KEEP;
}

void MainWidget::PositionEvent(QMouseEvent *event)
{
    m_yPosition = MousePosition(event->pos());
    switch(m_yPosition)
    {
        case DRAG_LOCK_LEFT:
        {
            setCursor(Qt::SizeHorCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_LEFTTOP:
        {
            setCursor(Qt::SizeFDiagCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_LEFTBOTTOM:
        {
            setCursor(Qt::SizeBDiagCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_RIGHT:
        {
            setCursor(Qt::SizeHorCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_RIGHTTOP:
        {
            setCursor(Qt::SizeBDiagCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_RIGHTBOTTOM:
        {
            setCursor(Qt::SizeFDiagCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_TOP:
        {
            setCursor(Qt::SizeVerCursor);
            m_bPosition = true;
            break;
        }
        case DRAG_LOCK_BOTTOM:
        {
            setCursor(Qt::SizeVerCursor);
            m_bPosition = true;
            break;
        }
        default:
            setCursor(Qt::ArrowCursor);
            m_bPosition = false;
            break;
    }
}

void MainWidget::ShowWidget(MainWidget::DRAG_ABSOLUTE_POSITION position, QMouseEvent *event)
{
    switch(position)
    {
        case DRAG_LOCK_LEFT:
        {
            if(event->globalPos().rx() < this->pos().rx())
            {
                this->setGeometry( event->globalPos().rx(), this->pos().ry(), this->width() + (this->pos().rx() - event->globalPos().rx()), this->height());
            } else if(event->globalPos().rx() > this->pos().rx() && (this->width() - (event->globalPos().rx() - this->pos().rx())  > this->minimumWidth()))
            {
                this->setGeometry( event->globalPos().rx(), this->pos().ry(), this->width() - (event->globalPos().rx() - this->pos().rx()), this->height());
            }
            break;
        }
        case DRAG_LOCK_LEFTTOP:
        {
            if(event->globalPos().rx() < this->pos().rx() || event->globalPos().ry() < this->pos().ry())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if(event->globalPos().rx() < this->pos().rx())
                {
                    width = this->width() + (this->pos().rx() - event->globalPos().rx());
                    rx = event->globalPos().rx();
                }
                if(event->globalPos().ry() < this->pos().ry())
                {
                    height = this->height() + (this->pos().ry() - event->globalPos().ry());
                    ry = event->globalPos().ry();
                }
                this->setGeometry(rx, ry, width, height);
            } else if(event->globalPos().rx() > this->pos().rx() || event->globalPos().ry() > this->pos().ry())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if((this->width() - (event->globalPos().rx() - this->pos().rx())  > this->minimumWidth()))
                {
                    width = this->width() - (event->globalPos().rx() - this->pos().rx());
                    rx = event->globalPos().rx();
                } else {
                    width = this->minimumWidth();
                    rx = this->pos().rx() + (this->width() - width);
                }

                if(((this->pos().ry() + this->height()) - event->globalPos().ry()) >= this->minimumHeight())
                {
                    height = this->height() - (event->globalPos().ry() - this->pos().ry());
                    ry = event->globalPos().ry();
                } else {
                    height = this->minimumHeight();
                    ry = this->pos().ry() + (this->height() - height);
                }
                this->setGeometry(rx, ry, width, height);
            }
            break;
        }
        case DRAG_LOCK_LEFTBOTTOM:
        {
            if(event->globalPos().rx() < this->pos().rx() || event->globalPos().ry() > this->pos().ry() + this->height())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if(event->globalPos().rx() < this->pos().rx())
                {
                    width = this->width() + (this->pos().rx() - event->globalPos().rx());
                    rx = event->globalPos().rx();
                }
                if(event->globalPos().ry() > this->pos().ry() + this->height())
                {
                    height = event->globalPos().ry() - this->pos().ry();
                }
                this->setGeometry(rx, ry, width, height);
            }
            else if(event->globalPos().rx() > this->pos().rx() || event->globalPos().ry() > this->pos().ry())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if(event->globalPos().rx() > this->pos().rx() && (this->width() - (event->globalPos().rx() - this->pos().rx())  > this->minimumWidth()))
                {
                    width = this->width() - (event->globalPos().rx() - this->pos().rx());
                    rx = event->globalPos().rx();
                }

                if(this->height() > this->minimumHeight())
                {
                    height = this->height() - ((this->pos().ry() + this->height()) - event->globalPos().ry());
                }
                 this->setGeometry(rx, ry, width, height);
            }
            break;
        }
        case DRAG_LOCK_RIGHT:
        {
            if(event->globalPos().rx() > this->pos().rx() && this->width() >= this->minimumWidth())
            {
                this->setGeometry( this->pos().rx(), this->pos().ry(), event->globalPos().rx() - this->pos().rx(), this->height());
            }
            break;
        }
        case DRAG_LOCK_RIGHTTOP:
        {
            if(event->globalPos().rx() > this->pos().rx() && event->globalPos().ry() < this->pos().ry())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if(event->globalPos().rx() > this->pos().rx())
                {
                    width = event->globalPos().rx() - this->pos().rx();
                    //rx = event->globalPos().rx();
                }
                if(event->globalPos().ry() < this->pos().ry())
                {
                    height = this->height() + (this->pos().ry() - event->globalPos().ry());
                    ry = event->globalPos().ry();
                }
                this->setGeometry(rx, ry, width, height);
            } else if(event->globalPos().rx() > this->pos().rx() && event->globalPos().ry() > this->pos().ry())
            {
                int width = this->width(), height = this->height();
                int rx = this->pos().rx(), ry = this->pos().ry();
                if(event->globalPos().rx() - this->pos().rx()  > this->minimumWidth())
                {
                    width = event->globalPos().rx() - this->pos().rx();
                    //rx = event->globalPos().rx();
                }

                if(((this->pos().ry() + this->height()) - event->globalPos().ry()) >= this->minimumHeight())
                {
                    height = this->height() - (event->globalPos().ry() - this->pos().ry());
                    ry = event->globalPos().ry();
                }
                this->setGeometry(rx, ry, width, height);
            }
            break;
        }
        case DRAG_LOCK_RIGHTBOTTOM:
        {
        if(event->globalPos().rx() > this->pos().rx() || event->globalPos().ry() > this->pos().ry())
        {
            int width = this->width(), height = this->height();
            int rx = this->pos().rx(), ry = this->pos().ry();
            if(event->globalPos().rx() > this->pos().rx())
            {
                width = event->globalPos().rx() - this->pos().rx();
                //rx = event->globalPos().rx();
            }
            if(event->globalPos().ry() > this->pos().ry())
            {
                height = event->globalPos().ry() - this->pos().ry();
                //ry = event->globalPos().ry();
            }
            this->setGeometry(rx, ry, width, height);
        } else if(event->globalPos().rx() > this->pos().rx() || event->globalPos().ry() > this->pos().ry())
        {
            int width = this->width(), height = this->height();
            int rx = this->pos().rx(), ry = this->pos().ry();
            if((this->width() - (event->globalPos().rx() - this->pos().rx())  > this->minimumWidth()))
            {
                width = this->width() - (event->globalPos().rx() - this->pos().rx());
                rx = event->globalPos().rx();
            }

            if(((this->pos().ry() + this->height()) - event->globalPos().ry()) >= this->minimumHeight())
            {
                height = this->height() - (event->globalPos().ry() - this->pos().ry());
                ry = event->globalPos().ry();
            }
            this->setGeometry(rx, ry, width, height);
        }
            break;
        }
        case DRAG_LOCK_TOP:
        {
            if(event->globalPos().ry() < this->pos().ry())
            {
                this->setGeometry( this->pos().rx(), event->globalPos().ry(), this->width(), this->height() + (this->pos().ry() - event->globalPos().ry()));
            }
            else if(event->globalPos().ry() > this->pos().ry() && ((this->pos().ry() + this->height()) - event->globalPos().ry()) >= this->minimumHeight())
            {
                this->setGeometry( this->pos().rx(), event->globalPos().ry(), this->width(), this->height() - (event->globalPos().ry() - this->pos().ry()));
            }
            break;
        }
        case DRAG_LOCK_BOTTOM:
        {
            if(event->globalPos().ry() > this->pos().ry() && (event->globalPos().ry() - this->pos().ry()) >= this->minimumHeight())
            {
                this->setGeometry( this->pos().rx(), this->pos().ry(), this->width(), event->globalPos().ry() - this->pos().ry());
            }
            break;
        }
        default:
            break;
    }
}

void MainWidget::InitWidget()
{
    setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);
    ui->HeadWidget->setMouseTracking(true);
    ui->LeftWidget->setMouseTracking(true);
    ui->BottomWidget->setMouseTracking(true);
    ui->FunctionWidget->setMouseTracking(true);
    ui->Test->setMouseTracking(true);
    ui->Setting->setMouseTracking(true);
    ui->Log->setMouseTracking(true);
    ui->LOG->setMouseTracking(true);
    ui->CloseButtion->setMouseTracking(true);
    ui->MinmizeButton->setMouseTracking(true);
    ui->TestButton->setMouseTracking(true);
    ui->SettiongButton->setMouseTracking(true);
    ui->LogButton->setMouseTracking(true);
    ui->HelpButton->setMouseTracking(true);
    ui->MainWidget->setMouseTracking(true);

    QIcon icon = QIcon(":/image/Ico/Catlog1.ico");
    this->setWindowIcon(icon);
    m_yPosition = DRAG_KEEP;

    setStyleSheet("background-color:rgba(30,30,30,255)");
    ui->HeadWidget->setStyleSheet("background-color:rgba(60,60,60,255)");
    ui->BottomWidget->setStyleSheet("background-color:rgba(25,25,25,255)");
    ui->LeftWidget->setStyleSheet("background-color:rgba(51,51,51,255)");
    ui->FunctionWidget->setStyleSheet("background-color:rgba(30,30,30,255)");

    QVBoxLayout *Mian_Test = new QVBoxLayout(ui->Test);
    networkwidget = new NetWorkWidget(ui->Test);
    Mian_Test->addWidget(networkwidget);
    Mian_Test->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *Mian_Log = new QVBoxLayout(ui->Log);
    logwidget = new LogWidget(ui->Log);
    Mian_Log->addWidget(logwidget);
    Mian_Log->setContentsMargins(2, 0, 2, 0);

    QVBoxLayout *Mian_Setting = new QVBoxLayout(ui->Setting);
    settingwidget = new SettingWidget(ui->Setting);
    Mian_Setting->addWidget(settingwidget);
    Mian_Setting->setContentsMargins(0, 0, 0, 0);
    ui->TestButton->setToolTip("切换到压力测试 模式");
    ui->SettiongButton->setToolTip("切换到设置 模式");
    ui->LogButton->setToolTip("切换到日志 模式");
    ui->HelpButton->setToolTip("打开帮助文档");
    SetUiNetWork(networkwidget->GetUiWidget());
    networkwidget->SetUiSetting(settingwidget->GetUiWidget());
    networkwidget->SetBinButton(settingwidget->GetBinButton());
    networkwidget->SetTestButton(settingwidget->GetTestButton());
    networkwidget->SetUiMain(this->ui);
    //connect(networkwidget, SIGNAL(Send_Progress(int)), this, SLOT(on_ProgressBar_Run(int)));

    ui->FunctionWidget->setCurrentIndex(0);
    ui->ProgressBar->setVisible(false);
    ui->ProgressBar->setValue(0);

    ui->Hintlabel->setText("压力测试");
}

/*
void MainWidget::on_ProgressBar_Run(int num)
{
    qDebug() << "pro";
    ui->ProgressBar->setValue(num);
}
*/


void MainWidget::on_HelpButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/graycatya"));
    //qDebug() << "this HelpButton_clicked";
}

void MainWidget::on_TestButton_clicked()
{
    ui->Hintlabel->setText("压力测试");
    ui->FunctionWidget->setCurrentIndex(0);
}

void MainWidget::on_SettiongButton_clicked()
{
    ui->Hintlabel->setText("软件设置");
    ui->FunctionWidget->setCurrentIndex(1);
}

void MainWidget::on_LogButton_clicked()
{
    ui->Hintlabel->setText("日志查看");
    ui->FunctionWidget->setCurrentIndex(2);
}

void MainWidget::on_ProgressBar_valueChanged(int value)
{

}
