#pragma once
#include <SkyEngine/config/config.h>
#ifdef GLFW_LIB_ENABLE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <list>

/*
    MachineState
*/
enum ACTION_PLATFORM
{
    KEYBOARD,
    MOUSE,
    SCROLL,
    CURSOR
};

struct State
{
    ACTION_PLATFORM platform;
};

struct keyState : public State
{
    keyState(int kn, int ks, int ka, int km) : key_name(kn), key_scanCode(ks), key_action(ka), key_mode(km)
    {
        platform = ACTION_PLATFORM::KEYBOARD;
    };
    int key_name;
    int key_scanCode;
    int key_action;
    int key_mode;
};

struct mouseState : public State
{
    mouseState(int mb, int ma, int mm) : mouse_button(mb), mouse_action(ma), mouse_mods(mm)
    {
        platform = ACTION_PLATFORM::MOUSE;
    };
    int mouse_button;
    int mouse_action;
    int mouse_mods;
};

struct scrollState : public State
{
    scrollState(double x, double y) : scroll_xoffset(x), scroll_yoffset(y)
    {
        platform = ACTION_PLATFORM::SCROLL;
    };
    double scroll_xoffset;
    double scroll_yoffset;
};

struct cursorState : public State
{
    cursorState(int x, int y) : cursor_xpos(x), cursor_ypos(y)
    {
        platform = ACTION_PLATFORM::CURSOR;
    };
    int cursor_xpos;
    int cursor_ypos;
};

struct keyCore
{
    void add_state(State *state)
    {
        stateList.push_front(state);
    };

    std::list<State *> stateList;

    State *apply_state()
    {
        if (stateList.empty())
            return nullptr;
        return stateList.back();
    };
    void pop_state()
    {
        if (!stateList.empty())
            stateList.pop_back();
    };
};
/*
    Static class
*/
struct VulkanKeyboard
{
    VulkanKeyboard()
    {
        getApp(this);
    };

    static VulkanKeyboard *getApp(VulkanKeyboard *ptr = nullptr);

#ifdef GLFW_LIB_ENABLE
    void init_keyboard(GLFWwindow *window);
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
    static void mouse_callback(GLFWwindow *window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
#endif
    keyCore core;
};

