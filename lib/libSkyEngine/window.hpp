#pragma once

#include <SkyEngine/config/config.h>

#ifdef VK_USE_PLATFORM_XCB_KHR
#include <xcb/xcb.h>
#endif

#ifdef QT_LIB_ENABLE
#include "qt_plugin/vkwidget.h"
#include <QVulkanInstance>
#include <QWindow>
#else
#ifdef GLFW_LIB_ENABLE
#define GLFW_INCLUDE_VULKAN
#include "extension/tinygltf/stb_image.h"
#include <GLFW/glfw3.h>
#endif
#endif

#include <cstring>
#include <functional>
#include <iostream>
#include <utility>

#define MAGICK_WIDTH 800;
#define MAGICK_HEIGHT 600;

#ifdef VK_USE_PLATFORM_XCB_KHR

static inline xcb_intern_atom_reply_t *
intern_atom_helper(xcb_connection_t *conn, bool only_if_exists,
                   const char *str) {
  xcb_intern_atom_cookie_t cookie =
      xcb_intern_atom(conn, only_if_exists, strlen(str), str);
  return xcb_intern_atom_reply(conn, cookie, NULL);
}

#endif

class MWindow {
public:
  bool m_quit = false;

  struct Settings {
    bool fullScreen = false;
  } m_settings;

  uint32_t m_destWidth = 1280;
  uint32_t m_destHeight = 720;

  //
  MWindow() = default;

  ~MWindow() {
    if (!destroyed)
      destroy();
  }

  void destroy() {
#ifdef QT_LIB_ENABLE
    if (window)
      delete window;
#else
    glfwDestroyWindow(window);
    glfwTerminate();
    window = nullptr;
    _primary = nullptr;
    destroyed = true;
#endif
  }

  void setViewParam(int _width, int _height) {
    width = _width;
    height = _height;
  }

  void setIcon(std::string path) { icon_path = std::move(path); }

#ifndef QT_LIB_ENABLE

  void setResizeCallFunc(void (*aFunc)(GLFWwindow *, int, int)) {
    _func = aFunc;
  }

  void setCloseCallFunc(void (*aFunc)(GLFWwindow *)) { afunc = aFunc; }

#endif

  static void glfw_error_callback(int error, const char *description) {
    std::cout << "Error: " << description << " Error code: " << error
              << std::endl;
  }

#ifdef QT_LIB_ENABLE

  VkWidget *initWidget(QWidget *parent = nullptr) {
    vulkan_widget = new VkWidget(parent);
    return vulkan_widget;
  }

  void initWindow(const std::string &app_name, void *aThis,
                  QWidget *widget = nullptr) {
    if (widget == nullptr) {
      window = new QWindow();
      window->setSurfaceType(QSurface::VulkanSurface);
      window->show();
      window->resize(width, height);
#ifdef VK_USE_PLATFORM_WIN32_KHR
      m_window = reinterpret_cast<HWND>(window->winId());
#elif defined(VK_USE_PLATFORM_XCB_KHR)
      m_window = window->winId();
#endif
    } else {
//            vulkan_widget = new VkWidget(widget);
#ifdef VK_USE_PLATFORM_WIN32_KHR
      m_window = reinterpret_cast<HWND>(vulkan_widget->winId());
#elif defined(VK_USE_PLATFORM_XCB_KHR)
      m_window = vulkan_widget->winId();
#endif
      //            width = vulkan_widget->width();
      //            height = vulkan_widget->height();
      //            widget->resize(width, height);
    }
    // For custom window properties
//        window->setWindowFlags(Qt::CustomizeWindowHint |
//        Qt::FramelessWindowHint);
// Init xcb
#ifdef VK_USE_PLATFORM_WIN32_KHR
#elif defined(VK_USE_PLATFORM_XCB_KHR)
    const xcb_setup_t *setup;
    xcb_screen_iterator_t iter;
    int scr;

    m_connection = xcb_connect(NULL, &scr);
    if (m_connection == NULL) {
      printf("Could not find a compatible Vulkan ICD!\n");
      fflush(stdout);
      exit(1);
    }

    setup = xcb_get_setup(m_connection);
    iter = xcb_setup_roots_iterator(setup);
    while (scr-- > 0)
      xcb_screen_next(&iter);
    m_screen = iter.data;
    //////////SetupWindowForXCB
#ifdef VK_USE_XCB_WINDOW
    uint32_t value_mask, value_list[32];

    m_window = xcb_generate_id(m_connection);

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = m_screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_KEY_PRESS |
                    XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                    XCB_EVENT_MASK_POINTER_MOTION |
                    XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE;

    if (m_settings.fullScreen) {
      width = m_destWidth = m_screen->width_in_pixels;
      height = m_destHeight = m_screen->height_in_pixels;
    }

    xcb_create_window(m_connection, XCB_COPY_FROM_PARENT, m_window,
                      m_screen->root, 0, 0, width, height, 0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT, m_screen->root_visual,
                      value_mask, value_list);

    /* Magic code that will send notification when window is destroyed */
    xcb_intern_atom_reply_t *reply =
        intern_atom_helper(m_connection, true, "WM_PROTOCOLS");
    m_atom_wm_delete_window =
        intern_atom_helper(m_connection, false, "WM_DELETE_WINDOW");

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
                        (*reply).atom, 4, 32, 1,
                        &(*m_atom_wm_delete_window).atom);

    xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
                        XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8, app_name.size(),
                        app_name.data());

    free(reply);

    if (m_settings.fullScreen) {
      xcb_intern_atom_reply_t *atom_wm_state =
          intern_atom_helper(m_connection, false, "_NET_WM_STATE");
      xcb_intern_atom_reply_t *atom_wm_fullscreen =
          intern_atom_helper(m_connection, false, "_NET_WM_STATE_FULLSCREEN");
      xcb_change_property(m_connection, XCB_PROP_MODE_REPLACE, m_window,
                          atom_wm_state->atom, XCB_ATOM_ATOM, 32, 1,
                          &(atom_wm_fullscreen->atom));
      free(atom_wm_fullscreen);
      free(atom_wm_state);
    }

    xcb_map_window(m_connection, m_window);
#endif
#endif
#else
  void initWindow(const std::string &app_name, void *aThis) {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
      throw std::runtime_error("init function error");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    // take primary monitor
    _primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *_mode = glfwGetVideoMode(_primary);

    glfwWindowHint(GLFW_RED_BITS, _mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, _mode->refreshRate);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);

    window = glfwCreateWindow(
        width, height, app_name.data(), nullptr,
        nullptr); //(_mode->width, _mode->height, "Vulkan", _primary, nullptr)
    glfwSetWindowUserPointer(window, aThis);
    if (_func != nullptr)
      glfwSetFramebufferSizeCallback(window, _func);
    else
      throw std::runtime_error("callback function for resize not set");
    if (afunc != nullptr)
      glfwSetWindowCloseCallback(window, afunc);
    else
      throw std::runtime_error("callback function for close not set");
    // glfwMakeContextCurrent(window);
#endif
    /*!
     * Set Icon to window
     */
    if (!icon_path.empty())
      setWindowIcon(icon_path.c_str());
  }

  bool shouldClosed() {
#ifdef QT_LIB_ENABLE
    // TODO: Make function on qt base
    return false;
#else
    return glfwWindowShouldClose(window);
#endif
  }

#ifdef QT_LIB_ENABLE

  QWindow *getWindow() { return window; }

#else
  GLFWwindow *getWindow() { return window; }
#endif

  int getWidth() const {
#ifdef QT_LIB_ENABLE
    return vulkan_widget->width();
#else
    return width;
#endif
  }

  int getHeight() const {
#ifdef QT_LIB_ENABLE
    return vulkan_widget->height();
#else
    return height;
#endif
  }

  int *Width() { return &width; }

  int *Height() { return &height; }

  uint32_t *uWidth() { return (reinterpret_cast<uint32_t *>(&width)); }

  uint32_t *uHeight() { return (reinterpret_cast<uint32_t *>(&height)); }

  void fullScreenSwitch() {
    // GLFWmonitor *monitor = glfwGetPrimaryMonitor();
#ifdef QT_LIB_ENABLE
#else
    const GLFWvidmode *mode = glfwGetVideoMode(_primary);
    if (!_fullscreen) {
      // backup
      glfwGetWindowPos(window, &windPos[0], &windPos[1]);
      glfwGetWindowSize(window, &windSize[0], &windSize[1]);
      //
      glfwSetWindowMonitor(window, _primary, 0, 0, mode->width, mode->height,
                           mode->refreshRate);
      _fullscreen = true;
    } else {
      glfwSetWindowMonitor(window, nullptr, windPos[1], windPos[1], windSize[0],
                           windSize[1], mode->refreshRate);
      _fullscreen = false;
    }
#endif
  }

private:
  void setWindowIcon(const char *path) {
#ifdef QT_LIB_ENABLE
#else
    GLFWimage images[1];
    images[0].pixels =
        stbi_load(path, &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);
#endif
  }

  std::string icon_path; //../icons/engine.png
  // fullscreen flag
  bool _fullscreen = false;

#ifdef QT_LIB_ENABLE
  QWindow *window = nullptr;
  VkWidget *vulkan_widget{};
#endif
#if defined(VK_USE_PLATFORM_XCB_KHR)
public:
  bool m_prepared = false;
  xcb_connection_t *m_connection = nullptr;
  xcb_screen_t *m_screen = nullptr;
  xcb_window_t m_window = NULL;
  xcb_intern_atom_reply_t *m_atom_wm_delete_window = nullptr;
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
public:
  HWND m_window = NULL;
  HINSTANCE windowInstance{};
#endif
#ifdef GLFW_LIB_ENABLE
  GLFWwindow *window{};
  GLFWmonitor *_primary{};

  // callback functions
  void (*_func)(GLFWwindow *, int, int){};

  void (*afunc)(GLFWwindow *){};
#endif

private:
  bool destroyed = false;
  //
  int windPos[2]{};
  int windSize[2]{};
  int width = MAGICK_WIDTH;

  int height = MAGICK_HEIGHT;
};
