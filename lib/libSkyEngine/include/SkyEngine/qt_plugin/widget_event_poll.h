//
// Created by ubuntu on 14.05.24.
//

#ifndef VKSky_WIDGET_EVENT_POLL_H
#define VKSky_WIDGET_EVENT_POLL_H

#include <SkyEngine/config/config.h>

#include <mutex>
#ifdef QT_LIB_ENABLE
#include <QEvent>
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
    explicit VkQtEvent(QEvent *ev = nullptr, EventType t = EventType::EV_EMPTY, int cd = -1, int act = -1) {
        event = ev;
        type = t;
        code = cd;
        action = act;
    }

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
    void set_event(QEvent *event, EventType type = EventType::EV_EMPTY, int code = -1, int act = -1) {
        std::unique_lock<std::mutex> lk(mute);
        events_list.emplace_back(event, type, code, act);
    }

    VkQtEvent get_event_from_queue() {
        std::unique_lock<std::mutex> lk(mute);
        if (events_list.empty())
            return VkQtEvent();
        auto ev = events_list.front();

        return ev;
    }
    bool empty(){
        return events_list.empty();
    }
    void pop_event() {
        std::unique_lock<std::mutex> lk(mute);
        if (!events_list.empty())
            events_list.pop_front();
    }

private:
    std::list<VkQtEvent> events_list;
    std::mutex mute;
#endif
};

#endif //VKSky_WIDGET_EVENT_POLL_H
