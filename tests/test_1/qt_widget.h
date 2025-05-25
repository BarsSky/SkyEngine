//
// Created by ubuntu on 28.05.24.
//

#ifndef ZUR_QT_WIDGET_H
#define ZUR_QT_WIDGET_H

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>


#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QObject>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <SkyEngine/qt_plugin/vkwidget.h>
#include <QSpinBox>
#include <QCheckBox>
#include <QListWidget>
#include <QString>
#include "qt_interface.h"
///#include  <QStackedLayout>
#include  <QStackedWidget>

//#include "protocoller.h"




QT_BEGIN_NAMESPACE

namespace Ui {
    class MainWindow;
}

QT_END_NAMESPACE


class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QT_Interface *ptr, QWidget *parent = nullptr);

    ~MainWindow();

    void set_vulkan_widget(VkWidget *vulkan_widget);

    void magick_connect();

    QWidget *entropyWidget;

private:
    ///@brief Завязываем общение между виджетом и сценой вулкана
    // QWidget* central_widget;
    QGridLayout *layout;
    QStackedWidget *layoutStack;
    VkWidget *magick_widget;
    QT_Interface *mgk_ptr;

public slots:

signals:


private:
    ////////////////// VARIABLE /////////////////////////
    QString connection_address = "192.168.30.5";//"127.0.0.1";
    int port_value = 3010;

    Ui::MainWindow *ui = nullptr;

    void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    // std::vector<FCO_imitation *> imit_lists;
};

#endif //ZUR_QT_WIDGET_H
