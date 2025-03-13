//
// Created by ubuntu on 13.05.24.
//

#ifndef VKDISPLAY_VKWIDGET_H
#define VKDISPLAY_VKWIDGET_H

#include <SkyEngine/config/config.h>

#ifdef QT_LIB_ENABLE
#include <QMainWindow>
#include <QObject>
#include <QKeyEvent>
#endif

#include <thread>
#include <iostream>
#include <SkyEngine/export_import_magick.h>
#include <SkyEngine/qt_plugin/widget_event_poll.h>

class VkWidget
#ifdef QT_LIB_ENABLE
    : public QWidget
#endif
{ // VKWIDGET_EXPORT
#ifdef QT_LIB_ENABLE

  // Q_OBJECT

public:
    explicit VkWidget(QWidget *parent = nullptr);

    ~VkWidget() override;

    void set_size_ptr(int *_width, int *_height);

    bool check_event();

    void eventHandler();

public slots:

    void showEvent(QShowEvent *event) override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

private:
    int *vWidth = nullptr;
    int *vHeight = nullptr;
    EventPool pool;

    void resizeVulkanWindow();

    static void updateVulkanKeyboard(VkQtEvent event);

    static void updateVulkanMouse(VkQtEvent event);

    static void updateVulkanMWheel(VkQtEvent event);

    static void updateVulkanCursor(VkQtEvent event);

    static void close_window();

    static void show_window();
#endif
};

#endif // VKDISPLAY_VKWIDGET_H
