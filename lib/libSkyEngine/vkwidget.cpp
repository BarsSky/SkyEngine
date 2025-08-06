//
// Created by ubuntu on 13.05.24.
//

#include <SkyEngine/qt_plugin/vkwidget.h>
#include <SkyEngine/vk_sky.hpp>
#ifdef QT_LIB_ENABLE
#include <qwidget.h>

void VkWidget::updateVulkanKeyboard(VkQtEvent event) {
  vk_display::VKDisplay::getApp()->send_key_event(event.event, event.code,
                                                  event.action);
}

void VkWidget::updateVulkanMouse(VkQtEvent event) {
  vk_display::VKDisplay::getApp()->send_mouse_event(event.event);
}

void VkWidget::updateVulkanMWheel(VkQtEvent event) {
  vk_display::VKDisplay::getApp()->send_scroll_event(event.event, event.code);
}

void VkWidget::updateVulkanCursor(VkQtEvent event) {
  vk_display::VKDisplay::getApp()->send_cursor_event(event.event);
}

void VkWidget::close_window() {
  vk_display::VKDisplay::getApp()->done = true;
  vk_display::VKDisplay::getApp()->stop_engine();
  delete vk_display::VKDisplay::getApp();
}

void VkWidget::show_window() {
  static bool isRunning = false;
  if (!isRunning) {
    try {
      vk_display::VKDisplay::getApp()->run();
      isRunning = true;
    } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      //        return EXIT_FAILURE;
    }
  }
  //    if(magick::Magick::getApp()->prepared){
  //
  //    }
}

void VkWidget::resizeEvent(QResizeEvent *event) {
  pool.set_event(reinterpret_cast<QEvent *>(event), EventType::EV_RESIZE);
  QWidget::resizeEvent(event);
}

void VkWidget::resizeVulkanWindow() {
  vk_display::VKDisplay::getApp()->recreateWindow();
}

void VkWidget::mouseMoveEvent(QMouseEvent *event) {
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_CURSOR);
}

void VkWidget::wheelEvent(QWheelEvent *event) {
  auto y_scr = event->angleDelta().y();
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_WHEEL, y_scr);
}

void VkWidget::keyReleaseEvent(QKeyEvent *event) {
  auto type = event->type();
  auto key = event->key();
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_KEYBOARD, key,
                 type);
}

void VkWidget::mouseReleaseEvent(QMouseEvent *event) {
  setFocus();
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_MOUSE);
}

void VkWidget::mousePressEvent(QMouseEvent *event) {
  setFocus();
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_MOUSE);
}

void VkWidget::keyPressEvent(QKeyEvent *event) {
  auto type = event->type();
  auto key = event->key();
  pool.set_event(static_cast<QEvent *>(event), EventType::EV_KEYBOARD, key,
                 type);
}

void VkWidget::closeEvent(QCloseEvent *event) {
  std::cout << "check close event" << std::endl;
  // TODO: make wait flag for close accept
  close_window();
  QWidget::closeEvent(event);
}

void VkWidget::showEvent(QShowEvent *event) { show_window(); }

void VkWidget::eventHandler() {
  pool.move();
  while (!pool.empty()) {
    auto _ev = pool.get_event_from_queue();
    // if (_ev.event == nullptr)
    //   break;
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
}

bool VkWidget::check_event() { return pool.event_come(); }

VkWidget::~VkWidget() { std::cout << "remove" << std::endl; }

VkWidget::VkWidget(QWidget *parent):QWidget(parent) {
  if (!hasMouseTracking()) {
    setMouseTracking(true);
  }
  if (!isEnabled()) {
    setEnabled(true);
  }
  //    this->setTabletTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setFocus();
}
#endif
// #ifdef QT_GUI_LIB
// #include "moc_vkwidget.cpp"
// #endif
