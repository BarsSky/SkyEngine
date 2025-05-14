#pragma once

#include <SkyEngine/config/config.h>
// includes for vulkan sdk and view
#define _USE_MATH_DEFINES
// #ifdef _MSC_VER
// #define _MSVC_LANG 201103L
// #define __cplusplus 201103L
// #if defined(__cplusplus)
// #define VK_NULL_HANDLE nullptr
// #endif
// #endif
#ifdef QT_LIB_ENABLE
#include <QObject>
#include <QWidget>
#endif
#include <vulkan/vulkan.h>
// #include <xcb/xcb.h>

#define VK_NO_PROTOTYPES

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtx/hash.hpp>

// standart
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <set>
#include <unordered_map>
#include <thread>
#include <cstdint>

#if (__cplusplus > 201402L)
#include <filesystem>
#else
#ifdef __linux__
#include <experimental/filesystem>
#else
#if(_MSVC_LANG < 201402L)
#include <experimental/filesystem>
#else
#include <filesystem>
#endif
#endif


#endif

// tools
#include <SkyEngine/export_import_magick.h>
#include <SkyEngine/vk_sky_objcamera.hpp>
#include <SkyEngine/qt_plugin/vkwidget.h>
#include <SkyEngine/vk_sky_pipelineobject.hpp>
#include <SkyEngine/vk_sky_model.hpp>
#include <SkyEngine/vk_sky_uioverlay.hpp>
#include <SkyEngine/vk_sky_keyboard.hpp>

namespace vk_sky {
  struct toolConfig {
    std::string name;
    uint32_t major_version;
    uint32_t minor_version;
    uint32_t patch_version;
  };

  struct Screen {
    bool have_spot() const {
      if (*height == 0 || *width == 0)
        return false;
        if (*uHeight == 0 || *uWidth == 0)
        return false;
      return true;
    }

    int *width{};
    int *height{};
    uint32_t *uWidth{};
    uint32_t *uHeight{};
#ifdef GLFW_LIB_ENABLE
    GLFWwindow *window = nullptr;
#endif
  };

  enum ObjectFlags {
    BASE_LOAD = 0,
    LOAD_IN_THREAD = 1
  };

  class LIBSKYENGINE_EXPORT VKSky
#ifdef QT_LIB_ENABLE
        : public QObject
    {
        Q_OBJECT
#else
  {
#endif

  public:
    VKSky();

#ifdef QT_LIB_ENABLE
        ~VKDisplay() override;
#else
    virtual ~VKSky();
#endif

    //
    static VKSky *getApp(VKSky *ptr = nullptr);

    // fps lock
    struct {
      bool lockOn = true;
      float maxFPS = 60;
    } appFramerate;

    /**
     *
     * @param flag of recreate window for show new image
     */
    void set_recreate_event(bool flag);

    // function for start
    void run();

    bool done = false;

    bool stop_engine();
#ifdef QT_LIB_ENABLE
        // functions for static call from qt widget

        VkWidget *get_vk_widget(QWidget *parent = nullptr);

        void send_key_event(void *ev, int key, int type);

        void send_mouse_event(void *event);

        void send_cursor_event(void *event);

        void send_scroll_event(void *event, double y_offset);
#endif
    Object *createObject(const pipelineObject &pipeline, ObjectFlags flag = ObjectFlags::BASE_LOAD);

    Screen getScreen();

    float get_timer();

    void recreateWindow();

    void removeObject(Object *_obj);

    float fpsCounter;
    uint64_t frameCount = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp =
        std::chrono::high_resolution_clock::now();

    // Implementation of library
    class CImpl;

    /**
     * @brief Function for override assets prepare (models/textures/shaders)
     *
     */
    virtual void PrepareAssets();

    // function for update objects buffers value
    virtual void updateUniformBuffer();

    void recreateCommandBuffer();

    /** @brief (Pure virtual) Render function to be implemented by the sample application */
    virtual void render() = 0;

    // FUNCTIONS for keyboard override
#ifdef QT_LIB_ENABLE
        /**
         * @brief
         *
         * @param key
         * @param scancode
         * @param action
         * @param mods
         */
        virtual void magickKeyboard(QKeyEvent *event, int key, int type);

        /**
         * @brief
         *
         * @param x_offset
         * @param y_offset
         */
        virtual void magickScroll(QWheelEvent *event, double y_offset);

        /**
         * @brief
         *
         * @param button
         * @param action
         * @param mods
         */
        virtual void magickMouse(QMouseEvent *event);

        /**
         * @brief
         *
         * @param x_pos
         * @param y_pos
         */
        virtual void magickCursor(QMouseEvent *event);
#endif
#ifdef GLFW_LIB_ENABLE
    /**
     * @brief
     *
     */
    void checkKeys();

    //
    /**
     * @brief
     *
     * @param key
     * @param scancode
     * @param action
     * @param mods
     */
    virtual void magickKeyboard(int key, int scancode, int action, int mods);

    /**
     * @brief
     *
     * @param x_offset
     * @param y_offset
     */
    virtual void magickScroll(double x_offset, double y_offset);

    /**
     * @brief
     *
     * @param button
     * @param action
     * @param mods
     */
    virtual void magickMouse(int button, int action, int mods);

    /**
     * @brief
     *
     * @param x_pos
     * @param y_pos
     */
    virtual void magickCursor(double x_pos, double y_pos);

    /**
     * @brief
     *
     * @param overlay
     */
    virtual void OnUpdateUIOverlay(gui::UIOverlay *overlay);

    /**
     * @brief
     *
     * @param x_pos
     * @param y_pos
     * @param handled
     */
    virtual void mouseMoved(float x_pos, float y_pos, bool handled);
#endif
    /**
     * @brief Initialize version of app and engine (number of version/name)
     *
     */
    virtual void version_init();

    /**
     * @brief Set the first camera object
     *
     */
    virtual void set_first_camera();

  private:
    void updateBuffers();
#ifdef QT_LIB_ENALE
        VkWidget *vulkan_widget = nullptr;
#endif
#ifdef GLFW_LIB_ENABLE
    VulkanKeyboard magick_keyboard;
#endif
    // Static callback on Close Window

    static void quit_from_engine(vk_sky::VKSky *obj);

    struct {
      bool left = false;
      bool right = false;
      bool middle = false;
    } mouseButtons;

    struct Scroll {
      bool up = false;
      bool down = false;
    } m_scroll;

    float m_distance = 0.f;
    glm::vec2 mousePosition{};

    std::unique_ptr<CImpl> u_ptr_;

    bool is_recreate = false;

  protected:
    std::thread magick_thread;
    toolConfig appConfig, engConfig;
    Screen current_window;
    ObjCamera camera;

    /**
     * @brief
     *
     */
    void mainLoop();

    // base frame function draw
    void drawFrame();

    void prepareObjectType();

    void visible_ui(bool flag) const;

    /**
     * @brief
     *
     * @param messageSeverity
     * @param messageType
     * @param pCallbackData
     * @param pUserData
     * @return VKAPI_ATTR
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData);
  };
};
