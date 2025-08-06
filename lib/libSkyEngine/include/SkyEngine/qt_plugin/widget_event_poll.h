//
// Created by ubuntu on 14.05.24.
//

#ifndef VKDISPLAY_WIDGET_EVENT_POLL_H
#define VKDISPLAY_WIDGET_EVENT_POLL_H

#include <array>
#include <chrono>
#include <cstddef>
#include <SkyEngine/config/config.h>

#include <mutex>
#include <thread>
#ifdef QT_LIB_ENABLE
#include <qevent.h>
#include <QEvent>
#include <QResizeEvent>
#endif
#include <list>

enum EventType {
  EV_EVENT,
  EV_RESIZE,
  EV_CLOSE,
  EV_KEYBOARD,
  EV_MOUSE,
  EV_WHEEL,
  EV_CURSOR,
  EV_EMPTY
};

struct VkQtEvent {
#ifdef QT_LIB_ENABLE
  explicit VkQtEvent(QEvent *ev = nullptr, EventType t = EventType::EV_EMPTY,
                     int cd = -1, int act = -1) {
    event = nullptr;
    switch (t) {
    case EventType::EV_RESIZE:
      event = new QResizeEvent(*(dynamic_cast<const QResizeEvent *>(ev)));
      break;
    case EventType::EV_CLOSE:
      event = new QCloseEvent(*(dynamic_cast<const QCloseEvent *>(ev)));
      break;
    case EventType::EV_KEYBOARD:
      event = new QKeyEvent(*(dynamic_cast<const QKeyEvent *>(ev)));
      break;
    case EventType::EV_MOUSE:
      event = new QMouseEvent(*(dynamic_cast<const QMouseEvent *>(ev)));
      break;
    case EventType::EV_WHEEL:
      event = new QWheelEvent(*(dynamic_cast<const QWheelEvent *>(ev)));
      break;
    case EventType::EV_CURSOR:
      event = new QMouseEvent(*(dynamic_cast<const QMouseEvent *>(ev)));
      break;
    case EventType::EV_EVENT:
      break;
    }
    type = t;
    code = cd;
    action = act;
  }

  void remove_Event() const { delete event; }

  ~VkQtEvent() {}

  QEvent *event;
#else
  void *event;
#endif
  EventType type;
  int code{};
  int action{};
};

struct EventPool {
#ifdef QT_LIB_ENABLE
  void set_event(QEvent *event, EventType type = EventType::EV_EMPTY,
                 int code = -1, int act = -1) {
    std::unique_lock<std::mutex> lk(fill_mute);
    events_list_array.at(fill).emplace_back(event, type, code, act);
  }

  VkQtEvent get_event_from_queue() {
    if (events_list.empty())
      return VkQtEvent();
    auto ev = events_list.front();

    return ev;
  }
  bool event_come() { return events_list_array.at(fill).empty(); }

  bool empty() { return events_list.empty(); }

  void pop_event() {
    if (!events_list.empty()) {
      events_list.front().remove_Event();
      events_list.pop_front();
    }
  }
  void move() {
    std::unique_lock<std::mutex> lk(fill_mute);
    events_list = events_list_array.at(fill);
    events_list_array.at(fill).clear();
    if (fill < 1) {
      fill++;
    } else {
      fill = 0;
    }
  }
  ~EventPool() {
    if (!events_list.empty()) {
      for (const auto& list : events_list) {
        list.remove_Event();
      }
    }
    for (const auto &arr : events_list_array) {
      if (!arr.empty()) {
        for (const auto& list : events_list) {
          list.remove_Event();
        }
      }
    }
  }

private:
  std::list<VkQtEvent> events_list;
  std::array<std::list<VkQtEvent>, 2> events_list_array;
  std::mutex mute, fill_mute;
  uint8_t fill = 0;
#endif
};

#endif // VKDISPLAY_WIDGET_EVENT_POLL_H
