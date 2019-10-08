#include "UiWidget/mainwidget.h"
//#include "UiWidget/networkwidget.h"
#include <QApplication>
#include "Log/CatLog.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CatLog::Log::Instance();
    CATLOG(CatLog::Log::LEVEL_INFO, "Start ok!", "");
    MainWidget w;
    w.show();
    return a.exec();
}
