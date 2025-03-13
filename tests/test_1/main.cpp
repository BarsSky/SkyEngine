#include <QApplication>
#include "qt_widget.h"
#include <SkyEngine/config/config.h>
#include <QWindow>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
#if defined(__unix) || defined(__unix__) || defined(__linux) || defined(__linux__)
    QApplication::setAttribute(Qt::AA_ForceRasterWidgets, false);
#endif
    auto *app = new QT_Interface;
    MainWindow window(app);

    window.resize(1024, 768);

    window.set_vulkan_widget(app->get_vk_widget(window.entropyWidget)); //&window
    window.show();
    // window.entropyWidget->controlPanel->setAnimation();
    // window.magick_connect();

    //  QWindow* test = new QWindow();
    // // //   test.se
    //  test->setOpacity(0.5);
    //  test->show();
    return a.exec();
}