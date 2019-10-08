#include "catiplineedit.h"
#include <QPainter>
#include <QDebug>
#include <QRegExp>
#include <QRegExpValidator>
#include <QKeyEvent>
#include <QMouseEvent>

CatIpLineedit::CatIpLineedit(QWidget *parent) :QWidget(parent)
{
    QRegExp rx("(2[0-5]{2}|2[0-4][0-9]|1?[0-9]{1,2})");
    m_BackGround  = "transparent";
    m_Frame = QColor(Qt::black);
    m_Dot = QColor(Qt::black);
    m_nFrameWidth = 2;
    m_nCircleDia = 4;
    m_TextColor = "gray";
    m_TextFamily = "Timers";
    m_nTextSize = 10;
    m_MousePreEvent = false;
    setMinimumSize(QSize(150, 15));
    setStyleSheet("background-color:"+m_BackGround);

    m_pHBoxLayout = new QHBoxLayout(this);
    m_pHBoxLayout->setSpacing(0);
    m_pHBoxLayout->setContentsMargins(0, 0, 0, 0);

    for(int i = 0; i < 4; i++)
    {
        QLineEdit *lineedit = new QLineEdit(this);
        lineVector.push_back(lineedit);

        lineVector[i]->setFrame(false);
        lineVector[i]->setMaxLength(3);
        lineVector[i]->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        lineVector[i]->installEventFilter(this);
        lineVector[i]->setStyleSheet(QString("background:transparent;border-width:0;border-style:outset;")+"color:"+m_TextColor);
        lineVector[i]->setValidator(new QRegExpValidator(rx, this));
        lineVector[i]->setFont(QFont(m_TextFamily , m_nTextSize,  QFont::Bold) );
        m_pHBoxLayout->addWidget(lineVector[i]);
        if(i < 3)
        {
            QSpacerItem* spaceritem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
            SpacerItemVector.push_back(spaceritem);
            m_pHBoxLayout->addItem(spaceritem);
        }
        connect(lineVector[i], SIGNAL(selectionChanged()), this, SLOT(MouseSetFrameColor()));
    }
}

CatIpLineedit::~CatIpLineedit()
{
    for(int i = 0; i < 4; i++)
    {
        if(lineVector[i] != nullptr)
        {
            delete lineVector[i];
            lineVector[i] = nullptr;
        }
    }
}

void CatIpLineedit::SetText(const QString &str)
{
     QRegExp rx2("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
     if(!rx2.exactMatch(str))
     {
         qDebug() << "SetText...error";
     }
     else {
         QStringList  iplist = str.split(".");
         for(int i = 0; i < 4; i++)
         {
            lineVector[i]->setText(iplist.at(i));
         }
     }
}

QString CatIpLineedit::Text() const
{
    QString temp;
    for(int i = 0; i < 4; i++)
    {
        if(lineVector[i]->text() == "")
        {
            return "";
        }
        temp += lineVector[i]->text();
        if(i < 3)
            temp += ".";
    }
    return temp;
}

void CatIpLineedit::SetBackGround_Color(QString color)
{
    m_BackGround = color;
    repaint();
}

void CatIpLineedit::SetText_Color(QString color)
{
    m_TextColor = color;
    repaint();
}

void CatIpLineedit::SetText_Family(QString family)
{
    m_TextFamily = family;
    repaint();
}

void CatIpLineedit::SetFrame_Color(QColor color)
{
    m_Frame = color;
    repaint();
}

void CatIpLineedit::SetGetFrameMouseEvent_Color(QColor color)
{
    m_FrameMouseEvent = color;
    repaint();
}

void CatIpLineedit::SetDot_Color(QColor color)
{
    m_Dot = color;
    repaint();
}

void CatIpLineedit::SetFrame_Width(int width)
{
    m_nFrameWidth = width;
    repaint();
}

void CatIpLineedit::SetSetCircle_Dia(int dia)
{
    m_nCircleDia = dia;
    repaint();
}

void CatIpLineedit::SetText_Size(int size)
{
    m_nTextSize = size;
    repaint();
}

void CatIpLineedit::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    setStyleSheet("background-color:"+m_BackGround);
    for(int i = 0; i < 4; i++)
    {
        lineVector[i]->setStyleSheet(QString("background:transparent;border-width:0;border-style:outset;")+"color:"+m_TextColor);
        lineVector[i]->setFont(QFont(m_TextFamily , m_nTextSize,  QFont::Bold) );
    }
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制边框
    painter.save();
    QPen pen;
    pen.setColor(QColor(m_Frame));
    pen.setWidth(m_nFrameWidth);
    painter.setPen(pen);
    QRect sliderRect(0,0,this->width(), this->height());
    painter.drawRoundedRect(sliderRect, 5, 5);
    painter.restore();

    //绘制原点
    QBrush brush;
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    brush.setColor(m_Dot);
    painter.setBrush(brush);

    painter.save();
    painter.drawEllipse(width()/4 - m_nCircleDia/2, height()/2 - m_nCircleDia/2, m_nCircleDia, m_nCircleDia);
    painter.restore();

    painter.save();
    painter.drawEllipse(width()/2 - m_nCircleDia/2, height()/2 - m_nCircleDia/2, m_nCircleDia, m_nCircleDia);
    painter.restore();

    painter.save();
    painter.drawEllipse(width() - (width()/4) - m_nCircleDia/2, height()/2 - m_nCircleDia/2, m_nCircleDia, m_nCircleDia);
    painter.restore();

}

bool CatIpLineedit::eventFilter(QObject *obj, QEvent *event)
{
    if(children().contains(obj) && QEvent::KeyPress == event->type())
    {

        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
        QLineEdit *pEdit = qobject_cast<QLineEdit *>(obj);

        switch (keyEvent->key())
        {
        case Qt::Key_0:
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
        {
            QString strText = pEdit->text();
            if (pEdit->selectedText().length())
            {
                pEdit->text().replace(pEdit->selectedText(), QChar(keyEvent->key()));
            }
            else if (strText.length() == 3 || (strText.length() < 3 && strText.toInt() * 10 > 255))
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 3)
                {
                    lineVector[index + 1]->setFocus();  //调整光标
                    lineVector[index + 1]->selectAll();
                }
            }
            else if (strText.length() == 2 && strText.toInt() * 10 < 255)
            {
                if (Qt::Key_0 == keyEvent->key() && strText.toInt())
                {
                    pEdit->setText(strText.insert(pEdit->cursorPosition(), QChar(Qt::Key_0)));
                }
            }
            return QWidget::eventFilter(obj, event);
        }
        case Qt::Key_Backspace:
        {
            QString strText = pEdit->text();
            if (strText.length() && pEdit->cursorPosition())
            {
                int index = getIndex(pEdit);
                //qDebug() << "index " << index;
                QString str = static_cast<QString>(lineVector[index]->text());
                str.chop(1);
                lineVector[index]->setText(str);
                if (index != -1 && index != 0 && lineVector[index]->text().length() == 0)
                {
                    lineVector[index - 1]->setFocus();
                    int length = lineVector[index - 1]->text().length();
                    lineVector[index - 1]->setCursorPosition(length ? length : 0);
                }
            } else {
                int index = getIndex(pEdit);
                lineVector[index - 1]->setFocus();
                int length = lineVector[index - 1]->text().length();
                lineVector[index - 1]->setCursorPosition(length ? length : 0);
            }
            return true;
        }
        case Qt::Key_Left:
        {
            if (!pEdit->cursorPosition())
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 0)
                {
                    lineVector[index - 1]->setFocus();
                    int length = lineVector[index - 1]->text().length();
                    lineVector[index - 1]->setCursorPosition(length ? length : 0);
                }
            }
            return true;
        }
        case Qt::Key_Right:
        {
            if (pEdit->cursorPosition() == pEdit->text().length())
            {
                int index = getIndex(pEdit);
                if (index != -1 && index != 3)
                {
                    lineVector[index + 1]->setFocus();
                    lineVector[index + 1]->setCursorPosition(0);
                }
            }
            return true;
        }
        default:
            break;
        }
    } else if(children().contains(obj) && QEvent::InputMethod == event->type())
    {
        //中文输入法状态下
        QInputMethodEvent *keyEvent = dynamic_cast<QInputMethodEvent *>(event);
        QLineEdit *pEdit = qobject_cast<QLineEdit *>(obj);
        QString strText = pEdit->text();
        QString strInput = keyEvent->commitString();
        //qDebug() << "strInput" << strInput;
        if (pEdit->selectedText().length())
        {
            pEdit->text().replace(pEdit->selectedText(), strInput);
        }
        else if ((strText.length() == 3) || ((strText.length() < 3) && (strText.toInt() * 10 > 255)))
        {
            int index = getIndex(pEdit);
            if (index != -1 && index != 3)
            {
                lineVector[index + 1]->setFocus();  //调整光标
                lineVector[index + 1]->selectAll();
            }
        }
        else if (strText.length() == 2 && strText.toInt() * 10 < 255)
        {
            if ("0" == strInput && strText.toInt())
            {
                pEdit->setText(strText.insert(pEdit->cursorPosition(), QChar(Qt::Key_0)));
            }
        }
    }
    return false;
}

void CatIpLineedit::SetBackGroundColor(QString color)
{
    m_BackGround = color;
    repaint();
}

void CatIpLineedit::SetTextColor(QString color)
{
    m_TextColor = color;
    repaint();
}

void CatIpLineedit::SetTextFamily(QString family)
{
    m_TextFamily = family;
    repaint();
}

void CatIpLineedit::SetFrameColor(QColor color)
{
    m_Frame = color;
    repaint();
}

void CatIpLineedit::SetFrameMouseEventColor(QColor color)
{
    m_FrameMouseEvent = color;
    repaint();
}

void CatIpLineedit::SetDotColor(QColor color)
{
    m_Dot = color;
    repaint();
}

void CatIpLineedit::SetFrameWidth(int width)
{
    m_nFrameWidth = width;
    repaint();
}

void CatIpLineedit::SetCircleDia(int dia)
{
    m_nCircleDia = dia;
    repaint();
}

void CatIpLineedit::SetTextSize(int size)
{
    m_nTextSize = size;
    repaint();
}

QString CatIpLineedit::GetBackGroundColor()
{
    return m_BackGround;
}

QString CatIpLineedit::GetTextColor()
{
    return m_TextColor;
}

QString CatIpLineedit::GetTextFamily()
{
    return m_TextFamily;
}

QColor CatIpLineedit::GetFrameColor()
{
    return m_Frame;
}

QColor CatIpLineedit::GetFrameMouseEventColor()
{
    return m_FrameMouseEvent;
}

QColor CatIpLineedit::GetDotColor()
{
    return m_Dot;
}

int CatIpLineedit::GetFrameWidth()
{
    return m_nFrameWidth;
}

int CatIpLineedit::GetCircleDia()
{
    return m_nCircleDia;
}

int CatIpLineedit::GetTextSize()
{
    return m_nTextSize;
}


int CatIpLineedit::getIndex(QLineEdit *pEdit)
{
    int index = -1;
    for (int i = 0; i < 4; i++)
    {
        if (pEdit == lineVector[i])
            index = i;
    }
    return index;
}

void CatIpLineedit::MouseSetFrameColor()
{
    //qDebug() << "MouseSetFrameColor";
}

