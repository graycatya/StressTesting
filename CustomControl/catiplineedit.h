#ifndef CATIPLINEEDIT_H
#define CATIPLINEEDIT_H

#include <QWidget>
#include <QLineEdit>
#include <QHBoxLayout>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
class QDESIGNER_WIDGET_EXPORT CatIpLineedit : public QWidget
#else
class CatIpLineedit : public QWidget
#endif
{
    Q_OBJECT

    Q_PROPERTY(QString m_BackGround READ GetBackGroundColor WRITE SetBackGroundColor)
    Q_PROPERTY(QString m_TextColor READ GetTextColor WRITE SetTextColor)
    Q_PROPERTY(QString m_TextFamily READ GetTextFamily WRITE SetTextFamily)
    Q_PROPERTY(QColor m_Frame READ GetFrameColor WRITE SetFrameColor)
    Q_PROPERTY(QColor m_FrameMouseEvent READ GetFrameMouseEventColor WRITE SetFrameMouseEventColor)
    Q_PROPERTY(QColor m_Dot READ GetDotColor WRITE SetDotColor)
    Q_PROPERTY(int m_nFrameWidth READ GetFrameWidth WRITE SetFrameWidth)
    Q_PROPERTY(int m_nCircleDia READ GetCircleDia WRITE SetCircleDia)
    Q_PROPERTY(int m_nTextSize READ GetTextSize WRITE SetTextSize)

public:
    explicit CatIpLineedit(QWidget *parent = nullptr);
    ~CatIpLineedit();

    void SetText(const QString &str);
    QString Text( void ) const;

    void SetBackGround_Color(QString color);
    void SetText_Color(QString color);
    void SetText_Family(QString family);

    void SetFrame_Color(QColor color);
    void SetGetFrameMouseEvent_Color(QColor color);
    void SetDot_Color(QColor color);

    void SetFrame_Width(int width);
    void SetSetCircle_Dia(int dia);
    void SetText_Size(int size);

    QString GetBackGroundColor( void );
    QString GetTextColor( void );
    QString GetTextFamily( void );
    QColor GetFrameColor( void );
    QColor GetFrameMouseEventColor( void );
    QColor GetDotColor( void );

    int GetFrameWidth( void );
    int GetCircleDia( void );
    int GetTextSize( void );


protected:
    void paintEvent(QPaintEvent* event);
    bool eventFilter(QObject *obj, QEvent *event);

    int getIndex(QLineEdit *pEdit);


private:
    QVector<QLineEdit *> lineVector;
    QVector<QSpacerItem*> SpacerItemVector;
    QHBoxLayout* m_pHBoxLayout;

    QString m_BackGround;           //背景颜色
    QString m_TextColor;            //字体颜色
    QString m_TextFamily;           //字体家族
    QColor m_Frame;                 //边框颜色
    QColor m_FrameMouseEvent;       //边框鼠标事件颜色
    QColor m_Dot;                   //点颜色

    int m_nFrameWidth;              //边框线宽
    int m_nCircleDia;               //圆点直径
    int m_nTextSize;                //字体大小

    bool m_MousePreEvent;           //鼠标按下事件

signals:

public slots:
    void MouseSetFrameColor();
    void SetBackGroundColor(QString color);
    void SetTextColor(QString color);
    void SetTextFamily(QString family);
    void SetFrameColor(QColor color);
    void SetFrameMouseEventColor(QColor color);
    void SetDotColor(QColor color);
    void SetFrameWidth(int width);
    void SetCircleDia(int dia);
    void SetTextSize(int size);

};

#endif // CATIPLINEEDIT_H
