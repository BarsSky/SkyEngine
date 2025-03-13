#include <SkyEngine/vk_sky.hpp>
#include <cstddef>
#include "tools.hpp"
#include <SkyEngine/vk_sky_buffer.hpp>
#include "vk_sky_swapchain.hpp"
#include <SkyEngine/vk_sky_device.hpp>
#include <SkyEngine/vk_sky_pipelineobject.hpp>
#include "basemath.hpp"
#include "threadpool.hpp"
#include "vk_sky_keyboard.hpp"
#include "vk_sky_impl.h"

using namespace vk_sky;

static void write_log(char *log) {
  int count = 0;
  while (true) {
    if (log[count] != '\0') {
      std::cout << log[count];
      count++;
    } else
      break;
  }
  std::cout << std::endl;
}

VKSky::VKSky() /*: enableUI(true),
                         loadOnThread(1),
                         threadObjectCount(0),
                         useQueryPool(false) */
{
  u_ptr_ = std::make_unique<CImpl>();
  getApp(this);
  // create threads
  //    numThreads = std::thread::hardware_concurrency();
  //    assert(numThreads > 0);
  //    std::cout << "numThreads = " << numThreads << std::endl;
  //    threadPool.setThreadCount(numThreads);
  // clearValues.resize(2);
}

VKSky *VKSky::getApp(VKSky *ptr) {
  static VKSky *mPtr = nullptr;
  if (ptr)
    mPtr = ptr;
  return mPtr;
}

void VKSky::version_init() {
  engConfig.name = "MagickEngine";
  engConfig.major_version = 0;
  engConfig.minor_version = 1;
  engConfig.patch_version = 0;
  appConfig.name = "App";
  appConfig.major_version = 0;
  engConfig.minor_version = 1;
  engConfig.patch_version = 0;
}

void VKSky::set_first_camera() {
  camera.type = ObjCamera::CameraType::FIRST;
  camera.setPerspective(45.0f, (float) *current_window.width / (float) *current_window.height, 0.1f,
                        10000000.0f);
  camera.setTranslation(glm::vec3(0.0f, 0.0f, -1500.0f));
  camera.movementSpeed = 1520.f;
}

void VKSky::updateBuffers() {
  updateUniformBuffer();
  u_ptr_->prepareUI();
  OnUpdateUIOverlay(&u_ptr_->uiOverlay);
  u_ptr_->renderUI();
  u_ptr_->update();
}

void VKSky::PrepareAssets() {
}

void VKSky::prepareObjectType() {
  PrepareAssets();
  u_ptr_->prepareObjectType();
}

void VKSky::visible_ui(bool flag) const {
  u_ptr_->visible_ui(flag);
}

VKAPI_ATTR VkBool32 VKAPI_CALL VKSky::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

void VKSky::drawFrame() {
  if (u_ptr_->paused)
    return;
  auto tStart = std::chrono::high_resolution_clock::now();
  auto tDiff = std::chrono::duration<double, std::milli>(tStart - lastTimestamp).count();
  fpsCounter = 1000.f / (float) (tDiff);
  ////lock fps at 60
  if (fpsCounter > appFramerate.maxFPS & appFramerate.lockOn)
    return;

  u_ptr_->initial_update();

  u_ptr_->prepareFence();

  u_ptr_->endRender();

  render();

  u_ptr_->camera_update();

  updateBuffers();

  auto tEnd = std::chrono::high_resolution_clock::now();
  // frameCount++;

  // auto tDiff = std::chrono::duration<double, std::milli>(tEnd - lastTimestamp).count();
  // fpsCounter = 1000.f/(float)(tDiff);
  // lastTimestamp=tEnd;
  lastTimestamp = tStart;
}

void VKSky::updateUniformBuffer() {
}

Object *VKSky::createObject(const pipelineObject &pipeline, vk_sky::ObjectFlags flag) {
  return u_ptr_->createObject(pipeline, flag);
}

VKSky::~VKSky() {
}

#ifdef QT_LIB_ENABLE
void VKDisplay::magickKeyboard(QKeyEvent *event, int key, int type) // int key, int scancode, int action, int mods);
{
    auto a_action = event->type();
    auto a_key = event->key();
    switch (type)
    {
    case QEvent::KeyPress:
        if (camera.type == Camera::FIRST)
        {
            switch (key)
            {
            case Qt::Key_W:
                camera.keys.up = true;
                break;
            case Qt::Key_S:
                camera.keys.down = true;
                break;
            case Qt::Key_A:
                camera.keys.left = true;
                break;
            case Qt::Key_D:
                camera.keys.right = true;
                break;
            default:
                break;
            }
        }
        switch (key)
        {
        case Qt::Key_F2:

            //                    vDevice.crossWindow.fullScreenSwitch();
            break;
        case Qt::Key_J:
            if (camera.cinematic)
            {
                camera.cinematic = false;
                camera.m_zoom = 0;
            }
            else
            {
                camera.cinematic = true;
                camera.m_zoom = 5;
            }

            break;
        default:
            break;
        }
        break;
    case QEvent::KeyRelease:
        if (camera.type == Camera::FIRST)
        {
            switch (key)
            {
            case Qt::Key_W:
                camera.keys.up = false;
                break;
            case Qt::Key_S:
                camera.keys.down = false;
                break;
            case Qt::Key_A:
                camera.keys.left = false;
                break;
            case Qt::Key_D:
                camera.keys.right = false;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

void VKDisplay::magickScroll(QWheelEvent *event, double y_offset)
{
    if (camera.type == Camera::lookat || camera.type == Camera::lookat_v2)
    {
        // camera.fov -= y_offset/70.0f;
        camera.setZoom(y_offset * 1);
        // camera.position.z -= y_offset*1.0f;
        // camera.setPosition(camera.position);
    }
}

void VKDisplay::magickMouse(QMouseEvent *event) // int button, int action, int mods);
{
    switch (event->button())
    {
    case Qt::LeftButton:
        if (event->type() == QEvent::MouseButtonPress)
            mouseButtons.left = true;
        if (event->type() == QEvent::MouseButtonRelease)
            mouseButtons.left = false;
        break;
    case Qt::RightButton:
        if (event->type() == QEvent::MouseButtonPress)
            mouseButtons.right = true;
        if (event->type() == QEvent::MouseButtonRelease)
            mouseButtons.right = false;
        break;
    case Qt::MiddleButton:
        if (event->type() == QEvent::MouseButtonPress)
            mouseButtons.middle = true;
        if (event->type() == QEvent::MouseButtonRelease)
            mouseButtons.middle = false;
        break;
    default:
        break;
    }
}

void VKDisplay::magickCursor(QMouseEvent *event) // x_pos, double y_pos);
{
    auto dx = static_cast<float>(mousePosition.x - event->pos().x());
    auto dy = static_cast<float>(mousePosition.y - event->pos().y());

    bool handled = false;

    //    mouseMoved(static_cast<float>(x_pos), static_cast<float>(y_pos), handled);

    if (handled)
    {
        mousePosition = glm::vec2(static_cast<float>(event->pos().x()), static_cast<float>(event->pos().y()));
        return;
    }
    if (camera.type == Camera::FIRST)
    {
        if (mouseButtons.left)
        {
            camera.rotate(glm::vec3(-dy * camera.rotationSpeed,
                                    dx * camera.rotationSpeed, 0.0f));
            //    camera.moveCam(static_cast<int>(event->pos().x()),static_cast<int>(event->pos().y()));
        }
        if (mouseButtons.right)
        {
            camera.translate(glm::vec3(-0.0f, 0.0f, dy * .005f));
        }
        if (mouseButtons.middle)
        {
            camera.translate(glm::vec3(dx * 0.005f, -dy * 0.005f, 0.0f));
        }
    }
    if (camera.type == Camera::lookat || camera.type == Camera::lookat_v2)
    {
        if (mouseButtons.left)
        {
            // camera.rotate(glm::vec3(dy * camera.rotationSpeed,
            //                         -dx * camera.rotationSpeed, 0.0f));
            camera.dxAxsAngle = dx * (2 * M_PI / (float)*current_window.width);
            camera.dyAxsAngle = -dy * (M_PI / (float)*current_window.height);
            camera.moving();
            camera.update(0);
        }
        if (mouseButtons.right)
        {
            camera.dzAxsAngle = glm::sign(dx) * (dx * dx + dy * dy) * (2 * M_PI / (float)*current_window.width);
            camera.moving();
            camera.update(0);
        }
        if (mouseButtons.middle)
        {
        }
    }
    mousePosition = glm::vec2(static_cast<float>(event->pos().x()), static_cast<float>(event->pos().y()));
}
#endif
#ifdef GLFW_LIB_ENABLE

void VKSky::checkKeys() {
  keyState *_key;
  mouseState *_mouse;
  scrollState *_scroll;
  cursorState *_cursor;

  State *_state = magick_keyboard.core.apply_state();
  if (_state != nullptr) {
    switch (_state->platform) {
      case KEYBOARD:
        _key = reinterpret_cast<keyState *>(_state);
        magickKeyboard(_key->key_name, _key->key_scanCode, _key->key_action, _key->key_mode);
        break;
      case SCROLL:
        _scroll = reinterpret_cast<scrollState *>(_state);
        magickScroll(_scroll->scroll_xoffset, _scroll->scroll_yoffset);
        break;
      case MOUSE:
        _mouse = reinterpret_cast<mouseState *>(_state);
        magickMouse(_mouse->mouse_button, _mouse->mouse_action, _mouse->mouse_mods);
        break;
      case CURSOR:
        _cursor = reinterpret_cast<cursorState *>(_state);
        magickCursor(_cursor->cursor_xpos, _cursor->cursor_ypos);
        break;
      default:
        break;
    }
    magick_keyboard.core.pop_state();
  }
}

void VKSky::magickKeyboard(int key, int scancode, int action, int mods) {
  switch (action) {
    case GLFW_PRESS:
      //      if (camera.type == ObjCamera::FIRST) {
      switch (key) {
        case GLFW_KEY_W:
          camera.keys.up = true;
          break;
        case GLFW_KEY_S:
          camera.keys.down = true;
          break;
        case GLFW_KEY_A:
          camera.keys.left = true;
          break;
        case GLFW_KEY_D:
          camera.keys.right = true;
          break;
        default:
          break;
      }
    //      }
      switch (key) {
        case GLFW_KEY_F2:
          // u_ptr_->crossWindow.fullScreenSwitch();
          break;
        case GLFW_KEY_F1:
          u_ptr_->uiOverlay.visible = !u_ptr_->uiOverlay.visible;
          u_ptr_->uiOverlay.updated = true;
        default:
          break;
      }
      break;
    case GLFW_RELEASE:
      //      if (camera.type == ObjCamera::FIRST) {
      switch (key) {
        case GLFW_KEY_W:
          camera.keys.up = false;
          break;
        case GLFW_KEY_S:
          camera.keys.down = false;
          break;
        case GLFW_KEY_A:
          camera.keys.left = false;
          break;
        case GLFW_KEY_D:
          camera.keys.right = false;
          break;
        default:
          break;
      }
    //      }
      break;
    default:
      break;
  }
}

void VKSky::magickScroll(double x_offset, double y_offset) {
  camera.translate(glm::vec3(0.0f, 0.0f, static_cast<float>(y_offset)));
}

void VKSky::magickMouse(int button, int action, int mods) {
  switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
      if (action == GLFW_PRESS)
        mouseButtons.left = true;
      if (action == GLFW_RELEASE)
        mouseButtons.left = false;
      break;
    case GLFW_MOUSE_BUTTON_RIGHT:
      if (action == GLFW_PRESS)
        mouseButtons.right = true;
      if (action == GLFW_RELEASE)
        mouseButtons.right = false;
      break;
    case GLFW_MOUSE_BUTTON_MIDDLE:
      if (action == GLFW_PRESS)
        mouseButtons.middle = true;
      if (action == GLFW_RELEASE)
        mouseButtons.middle = false;
      break;
    default:
      break;
  }
}

void VKSky::magickCursor(double x_pos, double y_pos) {
  auto dx = static_cast<float>(mousePosition.x - x_pos);
  auto dy = static_cast<float>(mousePosition.y - y_pos);

  /// Отслеживаем если курсор расположена на форме интерфейса
  bool handled = false;
  if (u_ptr_->is_ui_enable()) {
    const ImGuiIO &io = ImGui::GetIO();
    handled = io.WantCaptureMouse && u_ptr_->uiOverlay.visible;
    u_ptr_->uiOverlay.updated = true;
  }
  /// Ввести схожую логику для моделей
  // Если модуль переноса и взаимодействия со всеми объектами сцены то

  mouseMoved(static_cast<float>(x_pos), static_cast<float>(y_pos), handled);

  if (handled) {
    mousePosition = glm::vec2(static_cast<float>(x_pos), static_cast<float>(y_pos));
    return;
  }

  if (mouseButtons.left) {
    camera.rotate(glm::vec3(dy * camera.rotationSpeed,
                            -dx * camera.rotationSpeed, 0.0f));
  }
  if (mouseButtons.right) {
    camera.translate(glm::vec3(-0.0f, 0.0f, dy * .005f));
  }
  if (mouseButtons.middle) {
    camera.translate(glm::vec3(-dx * 0.005f, -dy * 0.005f, 0.0f));
  }
  mousePosition = glm::vec2(static_cast<float>(x_pos), static_cast<float>(y_pos));
}

void vk_sky::VKSky::OnUpdateUIOverlay(gui::UIOverlay *overlay) {
}

void VKSky::mouseMoved(float x_pos, float y_pos, bool handled) {
}

#endif

void VKSky::mainLoop() {
  u_ptr_->impl_flush();

  // check qt widgets event
#ifdef QT_LIB_ENABLE
  while (!done)
  {
      while (!vulkan_widget->check_event())
          vulkan_widget->eventHandler();
#endif
#ifdef GLFW_LIB_ENABLE
  while (!u_ptr_->ready_to_close()) {
    glfwPollEvents();
    checkKeys();
#endif
    if (current_window.have_spot())
      drawFrame();
  }
  u_ptr_->endLoop();
}

#ifdef QT_LIB_ENABLE
VkWidget *VKDisplay::get_vk_widget(QWidget * parent)
{
    vulkan_widget = u_ptr_->get_vk_widget(parent);
    return vulkan_widget;
}
#endif

void VKSky::quit_from_engine(vk_sky::VKSky *obj) {
  obj->done = true;
}

bool VKSky::stop_engine() {
  if (magick_thread.joinable())
    magick_thread.join();
  return true;
}

#ifdef QT_LIB_ENABLE
void VKDisplay::send_key_event(void *ev, int key, int type)
{
    magickKeyboard(reinterpret_cast<QKeyEvent *>(ev), key, type);
}

void VKDisplay::send_mouse_event(void *event)
{
    magickMouse(reinterpret_cast<QMouseEvent *>(event));
}

void VKDisplay::send_cursor_event(void *event)
{
    magickCursor(reinterpret_cast<QMouseEvent *>(event));
}

void VKDisplay::send_scroll_event(void *event, double y_offset)
{
    magickScroll(reinterpret_cast<QWheelEvent *>(event), y_offset);
}
#endif

void vk_sky::VKSky::run() {
  version_init();
  // set parametrs to impl
  u_ptr_->version_init(&appConfig, &engConfig);
  u_ptr_->initWindow();
  current_window = u_ptr_->getScreen();
  u_ptr_->set_camera_ptr(&camera);
  prepareObjectType();
  set_first_camera();
#ifdef GLFW_LIB_ENABLE
  magick_keyboard.init_keyboard(current_window.window);
#endif

  u_ptr_->do_magick();

#ifdef QT_LIB_ENABLE
  magick_thread = std::thread(&VKDisplay::mainLoop, this);
#else
  mainLoop();
#endif
}

Screen vk_sky::VKSky::getScreen() {
  return u_ptr_->getScreen();
}

float vk_sky::VKSky::get_timer() {
  return u_ptr_->frameTimer;
}

void vk_sky::VKSky::set_recreate_event(bool flag) {
  is_recreate = flag;
}

void vk_sky::VKSky::recreateWindow() {
  u_ptr_->recreateSwapChain();
}

void VKSky::removeObject(Object *_obj) {
  u_ptr_->RemoveObject(_obj);
}

// #endif
#ifdef QT_LIB_ENABLE

#include "moc_vk_display.cpp"
#include "vk_display.hpp"

#endif
