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
#include <cstdint>
enum ACTION_PLATFORM : std::uint8_t { KEYBOARD, MOUSE, SCROLL, CURSOR };

struct State {
  ACTION_PLATFORM platform;
};

struct keyState : public State {
  keyState(int k_n, int k_s, int k_a, int k_m)
      : State(), key_name(k_n), key_scanCode(k_s), key_action(k_a),
        key_mode(k_m) {
    platform = ACTION_PLATFORM::KEYBOARD;
  };
  int key_name;
  int key_scanCode;
  int key_action;
  int key_mode;
};

struct mouseState : public State {
  mouseState(int m_b, int m_a, int m_m)
      : State(), mouse_button(m_b), mouse_action(m_a), mouse_mods(m_m) {
    platform = ACTION_PLATFORM::MOUSE;
  };
  int mouse_button;
  int mouse_action;
  int mouse_mods;
};

struct scrollState : public State {
  scrollState(double s_x, double s_y)
      : State(), scroll_xoffset(s_x), scroll_yoffset(s_y) {
    platform = ACTION_PLATFORM::SCROLL;
  };
  double scroll_xoffset;
  double scroll_yoffset;
};

struct cursorState : public State {
  cursorState(int c_x, int c_y) : State(), cursor_xpos(c_x), cursor_ypos(c_y) {
    platform = ACTION_PLATFORM::CURSOR;
  };
  int cursor_xpos;
  int cursor_ypos;
};

struct keyCore {
  void add_state(State *state) { stateList.push_front(state); };

  std::list<State *> stateList;

  auto apply_state() -> State * {
    if (stateList.empty()) {
      return nullptr;
    }
    return stateList.back();
  };
  void pop_state() {
    if (!stateList.empty()) {
      stateList.pop_back();
    }
  };
};
/*
    Static class
*/
struct VulkanKeyboard {
  VulkanKeyboard() { getApp(this); };

  static auto getApp(VulkanKeyboard *ptr = nullptr) -> VulkanKeyboard *;

#ifdef GLFW_LIB_ENABLE
  void init_keyboard(GLFWwindow *window);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mode);
  static void scroll_callback(GLFWwindow *window, double xoffset,
                              double yoffset);
  static void mouse_callback(GLFWwindow *window, int button, int action,
                             int mods);
  static void cursor_position_callback(GLFWwindow *window, double xpos,
                                       double ypos);
#endif
  keyCore core;
};
