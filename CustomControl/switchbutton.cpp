#include "switchbutton.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

SwitchButton::SwitchButton(QWidget *parent) : QWidget(parent)
{
    m_ButtonStyle = Circle;
    m_pTimer = nullptr;

    m_nBlockInterval = 0;       //滑块离背景的间隔
    m_nRectRadius = 0;          //圆角角度
    m_bChecked = false;            //是否选中
    m_bShowText = false;           //是否显示文字
    m_bShowCircle = false;         //显示小圆
    m_bAnimation = false;          //动画过渡
    m_bMouseoverevent = false;

    m_BgColorOn = QColor(Qt::green);        //打开时背景颜色
    m_BgColorOff = QColor(Qt::gray);       //关闭时背景颜色
    m_SliderColorOn = QColor(Qt::white);    //打开时滑块颜色
    m_SliderColorOff = QColor(Qt::white);   //关闭时滑块颜色
    m_TextColorOn = QColor(Qt::white);      //打开时文字颜色
    m_TextColorOff = QColor(Qt::white);     //关闭时文字颜色

    m_sTextOn = "启动";      //打开时显示的文字
    m_sTextOff = "关闭";     //关闭时显示的文字

    m_nStep = 2;            //滑块移动步长

    m_nCurrentX = 0;        //滑块当前位置

    m_nAnimatoffset = 0;

}

SwitchButton::~SwitchButton()
{
    if(m_pTimer != nullptr)
    {
        delete m_pTimer;
        m_pTimer = nullptr;
    }
}

SwitchButton::ButtonStyle SwitchButton::GetButtonStyle() const
{
    return m_ButtonStyle;
}

int SwitchButton::GetBlockInterval() const
{
    return m_nBlockInterval;
}

int SwitchButton::GetRectRadius() const
{
    return m_nRectRadius;
}

bool SwitchButton::GetChecked() const
{
    return m_bChecked;
}

bool SwitchButton::GetShowText() const
{
    return m_bShowText;
}

bool SwitchButton::GetShowCircle() const
{
    return m_bShowCircle;
}

bool SwitchButton::GetAnimation() const
{
    return m_bAnimation;
}

QColor SwitchButton::GetBgColorOn() const
{
    return m_BgColorOn;
}

QColor SwitchButton::GetBgColorOff() const
{
    return m_BgColorOff;
}

QColor SwitchButton::GetSliderColorOn() const
{
    return m_SliderColorOn;
}

QColor SwitchButton::GetSliderColorOff() const
{
    return m_SliderColorOff;
}

QColor SwitchButton::GetTextColorOn() const
{
    return m_TextColorOn;
}

QColor SwitchButton::GetTextColorOff() const
{
    return m_TextColorOff;
}

QString SwitchButton::GetTextOn() const
{
    return m_sTextOn;
}

QString SwitchButton::GetTextOff() const
{
    return m_sTextOff;
}

void SwitchButton::drawTextorRect(QPainter *painter)
{
    if(m_bShowText)
    {
        QPen pen;
        QFont font;
        int textX = 0;
        QColor color = m_bChecked ? m_TextColorOn : m_TextColorOff;
        QString text;
        int textwdith = static_cast<int>(this->width() * 0.2);
        font.setPixelSize(textwdith);
        pen.setColor(color);
        //int sliderWidth = qMin(width(), height()) - m_nBlockInterval * 2;

        if(m_bChecked)
        {
            textX = static_cast<int>(this->width() * 0.07);
            text = m_sTextOn;
        }
        else {
            textX = static_cast<int>(this->width() * 0.53);
            text = m_sTextOff;
        }

        QRect textRect(textX, 0, textwdith * 2, height());
        painter->setPen(pen);
        painter->setFont(font);
        painter->drawText(textRect, Qt::AlignCenter, text);

    } else if(m_bShowCircle)
    {
        int textX;
        if(m_bChecked)
        {
            textX = static_cast<int>(this->width()*0.20 - this->height()*0.4/2);
        }
        else {
            textX = static_cast<int>(this->width()*0.80 - this->height()*0.4/2);
        }
        QRect rect(textX, static_cast<int>((this->height() - this->height()*0.4)/2),
                   static_cast<int>(this->height()*0.4), static_cast<int>(this->height()*0.4));
        QPen pen;
        pen.setWidth(static_cast<int>(this->height()*0.1));
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->drawEllipse(rect);
    }
}

int SwitchButton::GetEndX()
{
    int currentX = 0;
    switch(m_ButtonStyle)
    {
        case Slide_Roundrect:
            currentX = this->width() - this->width() / 2;
            break;
        case Slide_InCircle:
            currentX = this->width() - this->height();
            break;
        case Slide_OutCIrcle:
            currentX = static_cast<int>(this->width() - (this->height() - this->height()*0.25) - (this->height()*0.25/2));
            break;
        default:
            currentX = 0;
            break;
    }
    return currentX;
}

void SwitchButton::SetButtonStyle(SwitchButton::ButtonStyle style)
{
    this->m_ButtonStyle = style;
    if(m_ButtonStyle == ButtonStyle::Slide_OutCIrcle)
    {
        m_nCurrentX = static_cast<int>(this->height()*0.25/2);
    } else {
        m_nCurrentX = 0;        //滑块当前位置
    }
    this->update();
}

void SwitchButton::SetBlockInterval(int blockinterval)
{
    this->m_nBlockInterval = blockinterval;
    this->update();
}

void SwitchButton::SetRectRadius(int rectradius)
{
    this->m_nRectRadius = rectradius;
    this->update();
}

void SwitchButton::SetChecked(bool checked)
{
    this->m_bChecked = checked;
    this->update();
}

void SwitchButton::SetShowText(bool showtext)
{
    this->m_bShowText = showtext;
    this->update();
}

void SwitchButton::SetShowCircle(bool showcircle)
{
    this->m_bShowCircle = showcircle;
    this->update();
}

void SwitchButton::SetAnimation(bool animation)
{
    this->m_bAnimation = animation;
    if(m_bAnimation)
    {
        if(m_pTimer == nullptr)
        {
            m_pTimer = new QTimer(this);
            connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerAnimation()));
        }
    } else {
        if(m_pTimer != nullptr)
        {
            delete m_pTimer;
            m_pTimer = nullptr;
        }
    }
    this->update();
}

void SwitchButton::SetBgColorOn(QColor color)
{
    this->m_BgColorOn = color;
    this->update();
}

void SwitchButton::SetBgColorOff(QColor color)
{
    this->m_BgColorOff = color;
    this->update();
}

void SwitchButton::SetSliderColorOn(QColor color)
{
    this->m_SliderColorOn = color;
    this->update();
}

void SwitchButton::SetSliderColorOff(QColor color)
{
    this->m_SliderColorOff = color;
    this->update();
}

void SwitchButton::SetTextColorOn(QColor color)
{
    this->m_TextColorOn = color;
    this->update();
}

void SwitchButton::SetTextColorOff(QColor color)
{
    this->m_TextColorOff = color;
    this->update();
}

void SwitchButton::SetTextOn(QString str)
{
    this->m_sTextOn = str;
    this->update();
}

void SwitchButton::SetTextOff(QString str)
{
    this->m_sTextOff = str;
    this->update();
}

void SwitchButton::timerAnimation()
{
    if(m_ButtonStyle != Circle)
    {
        if(m_bChecked)
        {
            if(m_nCurrentX >= 0 && m_nCurrentX <= GetEndX())
            {
                m_nCurrentX = m_nCurrentX + 3;
            }
            else {
                m_nCurrentX = GetEndX();
                m_pTimer->stop();
            }
        } else {
            if(m_nCurrentX >= 0 && m_nCurrentX <= GetEndX())
            {
                m_nCurrentX = m_nCurrentX - 3;
            }
            else {
                if(m_ButtonStyle == ButtonStyle::Slide_OutCIrcle)
                {
                    m_nCurrentX = static_cast<int>(this->height()*0.25/2);
                } else {
                    m_nCurrentX = 0;        //滑块当前位置
                }
                m_pTimer->stop();
            }
        }
    }
    else {
        if(m_bMouseoverevent)
        {
            if(m_nAnimatoffset >= 0 && m_nAnimatoffset <= qMax(this->width(), this->height())*0.25)
            {
                m_nAnimatoffset++;
            }
            else {
                m_nAnimatoffset = static_cast<int>(qMax(this->width(), this->height())*0.25);
                m_pTimer->stop();
            }
        }
        else {
            if(m_nAnimatoffset >= 0 && m_nAnimatoffset <= qMax(this->width(), this->height())*0.25)
            {
                m_nAnimatoffset--;
            }
            else {
                m_nAnimatoffset = 0;
                m_pTimer->stop();
            }
        }
    }
    repaint();
}

void SwitchButton::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bChecked = !m_bChecked;
        if(m_ButtonStyle != Circle)
        {
            if(!m_bAnimation)
            {
                if(m_bChecked)
                    m_nCurrentX = GetEndX();
                else {
                    if(m_ButtonStyle != ButtonStyle::Slide_OutCIrcle)
                        m_nCurrentX = 0;
                    else {
                        m_nCurrentX = static_cast<int>(this->height()*0.25);
                    }
                }
            }
            else {
                m_pTimer->start(10);
            }
        }
        else {

        }
    }
    emit change();
    repaint();
}

void SwitchButton::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    //qDebug() << "enterEvent";
    m_bMouseoverevent = true;
    if(m_bAnimation && m_ButtonStyle == Circle)
    {
        m_pTimer->start(10);
    }
    repaint();
}

void SwitchButton::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    //qDebug() << "leaveEvent";
    m_bMouseoverevent = false;
    if(m_bAnimation  && m_ButtonStyle == Circle)
    {
        m_pTimer->start(10);
    }
    repaint();
}




void SwitchButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    switch (m_ButtonStyle)
    {
        case ButtonStyle::Slide_Roundrect:
        {
            drawSlideRoundrect(&painter);
            break;
        }
        case ButtonStyle::Slide_InCircle:
        {
            drawSlideInCircle(&painter);
            break;
        }
        case ButtonStyle::Slide_OutCIrcle:
        {
            drawSlideOutCIrcle(&painter);
            break;
        }
        case ButtonStyle::Circle:
        {
            drawCircle(&painter);
            break;
        }
    }
}

//滑动圆角矩形
void SwitchButton::drawSlideRoundrect(QPainter *painter)
{
    int width = this->width();
    int height = this->height();
    painter->save();
    //绘制外框
    QColor BgColor = m_bChecked ? m_BgColorOn : m_BgColorOff;
    QRect rect(0, 0, width, height);
    painter->setPen(Qt::NoPen);
    painter->setBrush(BgColor);
    painter->drawRoundedRect(rect, m_nRectRadius, m_nRectRadius);
    //绘制文字and圆圈
    drawTextorRect(painter);
    painter->restore();

    painter->save();
    //绘制内按钮
    int sliderwidth = this->width() / 2 - m_nBlockInterval * 2;
    int sliderheight = this->height() - m_nBlockInterval * 2;
    painter->setPen(Qt::NoPen);
    QColor SliderColor = m_bChecked ? m_SliderColorOn : m_SliderColorOff;
    painter->setBrush(SliderColor);
    QRect sliderRect(m_nCurrentX + m_nBlockInterval, m_nBlockInterval, sliderwidth, sliderheight);
    painter->drawRoundedRect(sliderRect, m_nRectRadius, m_nRectRadius);
    painter->restore();
}

//内圆角
void SwitchButton::drawSlideInCircle(QPainter *painter)
{
    painter->save();
    int width = this->width();
    int height = this->height();
    QColor BgColor = m_bChecked ? m_BgColorOn : m_BgColorOff;
    QRect rect(0, 0, width, height);
    painter->setPen(Qt::NoPen);
    painter->setBrush(BgColor);
    int side = qMin(rect.width(), rect.height());
    //绘制外框
    QPainterPath pathleftellipse;
    pathleftellipse.addEllipse(rect.x(), rect.y(), side, side);

    QPainterPath pathrect;
    pathrect.addRect(rect.x() + side/2, rect.y(), rect.width() - side, rect.height() );

    QPainterPath pathrightellipse;
    pathrightellipse.addEllipse(rect.width() - side, rect.y(), side, side);


    painter->drawPath(pathleftellipse + pathrect + pathrightellipse);
    //绘制文字and圆圈
    drawTextorRect(painter);
    painter->restore();
    //绘制内按钮
    painter->save();
    //int sliderwidth = this->width() / 2 - m_nBlockInterval * 2;
    int sliderheight = this->height() - m_nBlockInterval * 2;
    painter->setPen(Qt::NoPen);
    QColor SliderColor = m_bChecked ? m_SliderColorOn : m_SliderColorOff;
    painter->setBrush(SliderColor);
    QRect sliderRect(m_nCurrentX + m_nBlockInterval, m_nBlockInterval, sliderheight, sliderheight);
    painter->drawEllipse(sliderRect);
    //(sliderRect, sliderheight, m_nRectRadius, m_nRectRadius);
    painter->restore();
}

// 外圆角
void SwitchButton::drawSlideOutCIrcle(QPainter *painter)
{
    painter->save();
    int width = this->width();
    int height = this->height();
    QColor BgColor = m_bChecked ? m_BgColorOn : m_BgColorOff;
    //绘制按钮条
    QPen penRect;
    //测试用外框
    //painter->drawRect(0,0, this->width(), this->height());
    QRect rect(static_cast<int>(height*0.35), static_cast<int>(height*0.3), \
               static_cast<int>(width - (height*0.35)*2), static_cast<int>(height - (height*0.6)));
    penRect.setColor(BgColor);
    painter->setBrush(BgColor);
    painter->setPen(penRect);
    painter->drawRoundedRect(rect, m_nRectRadius, m_nRectRadius);
    painter->restore();
    //绘制内按钮
    painter->save();
    QPen penEllipse;
    int interval = static_cast<int>(height*0.25);
    penEllipse.setWidth(interval);
    penEllipse.setColor(BgColor);
    painter->setPen(penEllipse);
    painter->setBrush(QColor(Qt::white));
    QRect rectell(m_nCurrentX, interval/2, height - interval, height - interval);
    painter->drawEllipse(rectell);
    painter->restore();
}

// 圆形按钮
void SwitchButton::drawCircle(QPainter *painter)
{
    painter->save();
    int width = this->width();
    int height = this->height();
    int linewidth = qMin(width, height);
    //绘制按钮
    QPen pen;
    pen.setWidth(1);
    QColor color;
    //painter->drawRect(0,0,this->width(), this->height());
    if(m_bChecked == true)
    {
        color = m_SliderColorOn;
        pen.setColor(QColor("#92bd6c"));
        painter->setPen(pen);
        if(m_bMouseoverevent)
        {
            painter->setBrush(QColor("#C0FF3E"));
        }
        else
        {
            painter->setBrush(QColor("#92bd6c"));
        }
        QRectF rectf(-(width/2) + m_nAnimatoffset/2, -(height/2) + m_nAnimatoffset/2, \
                     width - m_nAnimatoffset, height - m_nAnimatoffset);
        painter->translate((width/2), (height/2));
        painter->drawEllipse(rectf);   //绘制椭圆
        pen.setWidth(static_cast<int>(linewidth*0.08)); //设置线宽
        pen.setColor(QColor(Qt::white));
        painter->setPen(pen);
        painter->drawLine(0,0, 0, static_cast<int>(-(height/2)*0.75) + m_nAnimatoffset/2);
    }
    else {
        color = m_SliderColorOn;
        pen.setColor(QColor(Qt::gray));
        painter->setPen(pen);
        if(m_bMouseoverevent)
        {
            painter->setBrush(QColor("#DCDCDC"));
        }
        else
        {
            painter->setBrush(QColor(Qt::gray));
        }
        QRectF rectf(-(width/2) + m_nAnimatoffset/2, -(height/2) + m_nAnimatoffset/2, \
                     width - m_nAnimatoffset, height - m_nAnimatoffset);
        painter->translate((width/2), (height/2));
        painter->drawEllipse(rectf);   //绘制椭圆
        pen.setWidth(static_cast<int>(linewidth*0.08)); //设置线宽
        pen.setColor(QColor(Qt::white));
        painter->setPen(pen);
        painter->drawLine(0,0, 0, static_cast<int>(-(height/2)*0.75) + m_nAnimatoffset/2);
    }
    QRectF rectArc(-(width/2) + m_nAnimatoffset/2, -(height/2) + m_nAnimatoffset/2, \
                   width * 0.6 - m_nAnimatoffset, height * 0.6 - m_nAnimatoffset);
    painter->translate((width/2) - (width/2*0.6), (height/2)- (height/2*0.6));
    int startAngle = 50 * 16;   //圆弧起始位置
    int spanAngle = static_cast<int>(-280) * 16;  //圆弧结束位置
    painter->drawArc(rectArc, startAngle, spanAngle);
    painter->restore();

}
