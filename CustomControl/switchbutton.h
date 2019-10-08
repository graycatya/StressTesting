#ifndef SWITCHBUTTON_H
#define SWITCHBUTTON_H

#include <QWidget>
#include <QTimer>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
class QDESIGNER_WIDGET_EXPORT SwitchButton : public QWidget
#else
class SwitchButton : public QWidget
#endif
{
    Q_OBJECT
    Q_ENUMS(ButtonStyle)

    Q_PROPERTY(ButtonStyle m_pButtonStyle READ GetButtonStyle WRITE SetButtonStyle)
    Q_PROPERTY(int m_pBlockInterval READ GetBlockInterval WRITE SetBlockInterval)
    Q_PROPERTY(int m_pRectRadius READ GetRectRadius WRITE SetRectRadius)
    Q_PROPERTY(bool m_pChecked READ GetChecked WRITE SetChecked)
    Q_PROPERTY(bool m_pShowText READ GetShowText WRITE SetShowText)
    Q_PROPERTY(bool m_pShowCircle READ GetShowCircle WRITE SetShowCircle)
    Q_PROPERTY(bool m_pAnimation READ GetAnimation WRITE SetAnimation)

    Q_PROPERTY(QColor m_pBgColorOn READ GetBgColorOn WRITE SetBgColorOn)
    Q_PROPERTY(QColor m_pBgColorOff READ GetBgColorOff WRITE SetBgColorOff)
    Q_PROPERTY(QColor m_pSliderColorOn READ GetSliderColorOn WRITE SetSliderColorOn)
    Q_PROPERTY(QColor m_pSliderCOlorOff READ GetSliderColorOff WRITE SetSliderColorOff)
    Q_PROPERTY(QColor m_pTextColorOn READ GetTextColorOn WRITE SetTextColorOn)
    Q_PROPERTY(QColor m_pTextColorOff READ GetTextColorOff WRITE SetTextColorOff)

    Q_PROPERTY(QString m_pTextOn READ GetTextOn WRITE SetTextOn)
    Q_PROPERTY(QString m_pTextOff READ GetTextOff WRITE SetTextOff)

public:
    enum ButtonStyle{
        Slide_Roundrect = 0,  //圆角矩形
        Slide_InCircle = 1,   //内圆形
        Slide_OutCIrcle = 2,   //外圆形
        Circle = 3            //圆形
    };

public:
    explicit SwitchButton(QWidget *parent = nullptr);
    ~SwitchButton();

    ButtonStyle GetButtonStyle()    const;
    int GetBlockInterval()          const;
    int GetRectRadius()             const;
    bool GetChecked()               const;
    bool GetShowText()              const;
    bool GetShowCircle()            const;
    bool GetAnimation()             const;

    QColor GetBgColorOn()           const;
    QColor GetBgColorOff()          const;
    QColor GetSliderColorOn()       const;
    QColor GetSliderColorOff()      const;
    QColor GetTextColorOn()         const;
    QColor GetTextColorOff()        const;

    QString GetTextOn()             const;
    QString GetTextOff()            const;

protected:


private:
    ButtonStyle m_ButtonStyle;
    QTimer *m_pTimer;

    int m_nBlockInterval;       //滑块离背景的间隔
    int m_nRectRadius;          //圆角角度
    int m_nAnimatoffset;        //圆角按钮动画
    bool m_bChecked;            //是否选中
    bool m_bShowText;           //是否显示文字
    bool m_bShowCircle;         //显示小圆
    bool m_bAnimation;          //动画过渡
    bool m_bMouseoverevent;     //鼠标悬停


    QColor m_BgColorOn;        //打开时背景颜色
    QColor m_BgColorOff;       //关闭时背景颜色
    QColor m_SliderColorOn;    //打开时滑块颜色
    QColor m_SliderColorOff;   //关闭时滑块颜色
    QColor m_TextColorOn;      //打开时文字颜色
    QColor m_TextColorOff;     //关闭时文字颜色

    QString m_sTextOn;      //打开时显示的文字
    QString m_sTextOff;     //关闭时显示的文字

    int m_nStep;            //滑块移动步长
    int m_nCurrentX;        //滑块当前位置

private:
    void drawTextorRect(QPainter *painter);
    int GetEndX();

signals:
    void change();

public Q_SLOTS:
    void SetButtonStyle(ButtonStyle style);
    void SetBlockInterval(int blockinterval);
    void SetRectRadius(int rectradius);
    void SetChecked(bool checked);
    void SetShowText(bool showtext);
    void SetShowCircle(bool showcircle);
    void SetAnimation(bool animation);
    void SetBgColorOn(QColor color);
    void SetBgColorOff(QColor color);
    void SetSliderColorOn(QColor color);
    void SetSliderColorOff(QColor color);
    void SetTextColorOn(QColor color);
    void SetTextColorOff(QColor color);
    void SetTextOn(QString str);
    void SetTextOff(QString str);

private slots:
    void timerAnimation();

protected:
    void mousePressEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent* event);
    void drawSlideRoundrect(QPainter *painter);
    void drawSlideInCircle(QPainter *painter);
    void drawSlideOutCIrcle(QPainter *painter);
    void drawCircle(QPainter *painter);
};

#endif // SWITCHBUTTON_H
