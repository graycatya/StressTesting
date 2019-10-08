#include "settingwidget.h"
#include "ui_settingwidget.h"
#include <QDebug>
#include <QFileDialog>
#include <QRegExpValidator>
#include <QRegExp>

SettingWidget::SettingWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingUpWidget)
{
    ui->setupUi(this);
    m_pCatIpLineedit = nullptr;
    m_pBinButton = nullptr;
    m_pTestButton = nullptr;
    InitWidget();
}

SettingWidget::~SettingWidget()
{
    if(m_pCatIpLineedit != nullptr)
    {
        delete m_pCatIpLineedit;
        m_pCatIpLineedit = nullptr;
    }
    delete ui;
}

Ui::SettingUpWidget *SettingWidget::GetUiWidget()
{
    return ui;
}

SwitchButton *SettingWidget::GetBinButton()
{
    return m_pBinButton;
}

SwitchButton *SettingWidget::GetTestButton()
{
    return m_pTestButton;
}

void SettingWidget::InitWidget()
{
    ui->SendDatagroupBox->setChecked(true);
    ui->ReadDatagroupBox->setChecked(true);

    m_pBinButton = new SwitchButton(ui->BinButtonwidget);
    m_pBinButton->SetButtonStyle(SwitchButton::Slide_InCircle);
    m_pBinButton->setGeometry(0,0,50, 20);
    m_pBinButton->SetBlockInterval(2);
    m_pBinButton->SetAnimation(true);
    m_pTestButton = new SwitchButton(ui->TestButtonwidget);
    m_pTestButton->SetButtonStyle(SwitchButton::Slide_InCircle);
    m_pTestButton->setGeometry(0,0,50, 20);
    m_pTestButton->SetBlockInterval(2);
    m_pTestButton->SetAnimation(true);

    QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator *m_IPValidator = new QRegExpValidator(rx, this);
    ui->IplineEdit->setValidator(m_IPValidator);

    QRegExp regx("[0-9]+$");
    QValidator* validator = new QRegExpValidator(regx, this);
    ui->PostlineEdit->setValidator(validator);
    ui->ThreadNumlineEdit->setValidator(validator);
    ui->TimeOutlineEdit->setValidator(validator);
    ui->TestTimelineEdit->setValidator(validator);

}

void SettingWidget::on_comboBox_currentIndexChanged(const QString &arg1)
{
   if(arg1 == "HTTP")
   {
       ui->SettingstackedWidget->setCurrentIndex(1);
   } else {
       ui->SettingstackedWidget->setCurrentIndex(0);
   }
}

void SettingWidget::on_FilePathtoolButton_clicked()
{
    QString FilePath =  QFileDialog::getExistingDirectory(this, tr("Log Path"), "./");
    if(FilePath != "")
    {
        ui->FilePathlineEdit->setText(FilePath);
    }
}
