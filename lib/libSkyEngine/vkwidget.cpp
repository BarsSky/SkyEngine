//
// Created by ubuntu on 13.05.24.
//

#include <SkyEngine/qt_plugin/vkwidget.h>
#include <SkyEngine/vk_sky.hpp>

#ifdef QT_LIB_ENABLE
void VkWidget::updateVulkanKeyboard(VkQtEvent event) {
    vk_sky::VKSky::getApp()->send_key_event(event.event, event.code, event.action);
}

void VkWidget::updateVulkanMouse(VkQtEvent event) {
    vk_sky::VKSky::getApp()->send_mouse_event(event.event);
}
void VkWidget::updateVulkanMWheel(VkQtEvent event)
{
    vk_sky::VKSky::getApp()->send_scroll_event(event.event,event.code);
}

void VkWidget::updateVulkanCursor(VkQtEvent event) {
    vk_sky::VKSky::getApp()->send_cursor_event(event.event);
}

void VkWidget::close_window() {
    vk_sky::VKSky::getApp()->done = true;
    vk_sky::VKSky::getApp()->stop_engine();
}

void VkWidget::show_window() {
    static bool isRunning = false;
    if(!isRunning)
    {
        try {
            vk_sky::VKSky::getApp()->run();
            isRunning = true;
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }
}

VkWidget::~VkWidget() {
    std::cout << "remove" << std::endl;
}

void VkWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_RESIZE);
}

void VkWidget::resizeVulkanWindow() {
    if (vHeight && vWidth) {
        *vHeight = this->height();
        *vWidth = this->width();
    }
    vk_sky::VKSky::getApp()->recreateWindow();
}

void VkWidget::mouseMoveEvent(QMouseEvent *event) {
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_CURSOR);
}

void VkWidget::wheelEvent(QWheelEvent *event) {
    auto y_scr = event->angleDelta().y();
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_WHEEL, y_scr);
}

void VkWidget::keyReleaseEvent(QKeyEvent *event) {
    auto type = event->type();
    auto key = event->key();
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_KEYBOARD, key, type);
}

void VkWidget::mouseReleaseEvent(QMouseEvent *event) {
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_MOUSE);
}

void VkWidget::mousePressEvent(QMouseEvent *event) {
    setFocus();
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_MOUSE);
}

void VkWidget::keyPressEvent(QKeyEvent *event) {
    auto type = event->type();
    auto key = event->key();
    pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_KEYBOARD, key, type);

}

void VkWidget::closeEvent(QCloseEvent *event) {
    std::cout << "check close event" << std::endl;
    //TODO: make wait flag for close accept
    close_window();
    QWidget::closeEvent(event);
}

void VkWidget::showEvent(QShowEvent *event) {
    show_window();
}

void VkWidget::eventHandler() {
    auto _ev = pool.get_event_from_queue();
    if (_ev.event == nullptr)
        return;
    switch (_ev.type) {
        case EventType::EV_EVENT:
            break;
        case EventType::EV_KEYBOARD:
            updateVulkanKeyboard(_ev);
            break;
        case EventType::EV_MOUSE:
            updateVulkanMouse(_ev);
            break;
        case EventType::EV_CURSOR:
            updateVulkanCursor(_ev);
            break;
        case EventType::EV_RESIZE:
            resizeVulkanWindow();
            break;
        case EventType::EV_WHEEL:
            updateVulkanMWheel(_ev);
            break;
        case EventType::EV_CLOSE:
            break;
        case EventType::EV_EMPTY:
            break;
        default:
            break;
    }
    pool.pop_event();
}

bool VkWidget::check_event() {
    return pool.empty();
}

void VkWidget::set_size_ptr(int *_width, int *_height) {
    vHeight = _height;
    vWidth = _width;
}

VkWidget::VkWidget(QWidget *parent) {
    if (!hasMouseTracking())
        setMouseTracking(true);
    if (!isEnabled())
        setEnabled(true);
    setFocus();
}
#endif
