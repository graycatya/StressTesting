#ifndef PROGRESSARC_H
#define PROGRESSARC_H

#include <QWidget>

#ifdef quc
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#include <QtDesigner/QDesignerExportWidget>
#else
#include <QtUiPlugin/QDesignerExportWidget>
#endif
class QDESIGNER_WIDGET_EXPORT ProgressArc : public QWidget
#else
class ProgressArc : public QWidget
#endif
{
    Q_OBJECT
    Q_PROPERTY(int m_plineWidth READ GetLineWidth WRITE SetLineWidth)
    Q_PROPERTY(QColor m_plineColor READ GetLineColor WRITE SetLineColor)
    Q_PROPERTY(int m_pborderWidth READ GetBorderWidth WRITE SetBorderWidth)
    Q_PROPERTY(QColor m_pborderColor READ GetBorderColor WRITE SetBorderColor)
    Q_PROPERTY(QColor m_pbgColor READ GetBgColor WRITE SetBgColor)

public:
    explicit ProgressArc(QWidget *parent = nullptr);
    ~ProgressArc();
    void SetValue(int value);

protected:
    void paintEvent(QPaintEvent *event);
    void drawProress(QPainter *painter);

private:
    int     m_plineWidth;   //线条宽度
    QColor  m_plineColor;   //线条颜色
    int     m_pborderWidth; //边框宽度
    QColor  m_pborderColor; //边框颜色
    QColor  m_pbgColor;     //背景颜色

    int  value;          //当前值

public:
    int GetLineWidth()      const;
    QColor GetLineColor()   const;
    int GetBorderWidth()    const;
    QColor GetBorderColor() const;
    QColor GetBgColor()     const;


signals:
    void ProgressOutSlot();

public Q_SLOT:
    void SetLineWidth(int linewidth);
    void SetLineColor(const QColor &color);
    void SetBorderWidth(int borderwidth);
    void SetBorderColor(const QColor &bordercolor);
    void SetBgColor(const QColor &bgcolor);

public slots:
    void ProgressSlot(int value);
    void TestValueSlot();   //测试使用
};

#endif // PROGRESSARC_H
