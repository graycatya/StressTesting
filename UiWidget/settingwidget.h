#ifndef SETTINGWIDGET_H
#define SETTINGWIDGET_H

#include <QWidget>
#include "CustomControl/catiplineedit.h"
#include "CustomControl/switchbutton.h"
namespace Ui {
    class SettingUpWidget;
}

class SettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SettingWidget(QWidget *parent = nullptr);
    ~SettingWidget();
    Ui::SettingUpWidget* GetUiWidget( void );
    SwitchButton* GetBinButton( void );
    SwitchButton* GetTestButton( void );

private:
    void InitWidget( void );

private:
    Ui::SettingUpWidget *ui;
    CatIpLineedit* m_pCatIpLineedit;
    SwitchButton* m_pBinButton;
    SwitchButton* m_pTestButton;

signals:

public slots:
private slots:
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_FilePathtoolButton_clicked();
};

#endif // SETTINGWIDGET_H
