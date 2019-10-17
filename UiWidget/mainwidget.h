#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
//#include "NetThreadWork/tcptool.h"
#include <QMutex>

class LogWidget;
class NetWorkWidget;
class SettingWidget;

class ThreadPool;

namespace Ui {
class mainwindow;
}

namespace Ui {
class NetWorkWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    Q_ENUMS(DRAG_ABSOLUTE_POSITION)

public:
    enum DRAG_ABSOLUTE_POSITION
    {
        DRAG_KEEP = 0,          //保持不动
        DRAG_LOCK_LEFT,         //锁定左边坐标
        DRAG_LOCK_LEFTTOP,
        DRAG_LOCK_LEFTBOTTOM,

        DRAG_LOCK_RIGHT,        //锁定右边坐标
        DRAG_LOCK_RIGHTTOP,
        DRAG_LOCK_RIGHTBOTTOM,

        DRAG_LOCK_TOP,          //锁定上边坐标
        DRAG_LOCK_BOTTOM        //锁定下边坐标
    };
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
    Ui::mainwindow* GetUiWidget( void );
    void SetUiNetWork(Ui::NetWorkWidget * ui);

protected:
    void mousePressEvent(QMouseEvent *event);   //鼠标按下事件
    void mouseMoveEvent(QMouseEvent *event);    //鼠标移动事件
    void mouseReleaseEvent(QMouseEvent *event); //鼠标松开事件

private:
    //DRAG_ABSOLUTE_POSITION MousePosition(QPoint point);
    //void PositionEvent(QMouseEvent *event);
    //void ShowWidget(DRAG_ABSOLUTE_POSITION position, QMouseEvent *event);
    void InitWidget( void );

private:
    Ui::mainwindow *ui;
    Ui::NetWorkWidget *ui_net;
    LogWidget *logwidget;
    NetWorkWidget *networkwidget;
    SettingWidget *settingwidget;

    bool m_bPressed;
    bool m_bPosition;
    QPoint m_yWidgetPoint;
    QPoint m_yMousePoint;
    QPoint m_yNewPoint;
    ThreadPool *m_pThreadPool;
    DRAG_ABSOLUTE_POSITION m_yPosition;
    QMutex m_yPositionMutex;


signals:


public slots:
    //void on_ProgressBar_Run(int num);



private slots:
    void on_HelpButton_clicked();
    void on_TestButton_clicked();
    void on_SettiongButton_clicked();
    void on_LogButton_clicked();
    void on_ProgressBar_valueChanged(int value);
};

#endif // MAINWIDGET_H
