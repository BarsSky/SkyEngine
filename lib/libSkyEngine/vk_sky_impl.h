//
// Created by ubuntu on 30.07.24.
//

#ifndef PK_DISPLAY_VK_DISPLAY_IMPL_H
#define PK_DISPLAY_VK_DISPLAY_IMPL_H

#include <SkyEngine/vk_sky.hpp>
#include <SkyEngine/vk_sky_texture.hpp>
#include "initializer.hpp"
#include "tools.hpp"
#include "threadpool.hpp"
#include "vk_sky_keyboard.hpp"
#include <thread>
#include <chrono>
#include <SkyEngine/export_import_magick.h>
#ifdef QT_LIB_ENABLE
#include <QObject>
#endif

using namespace vk_sky;

struct cmdBufferRecThrdPrm {
  VkCommandBuffer commandBuffer;
  std::function<bool(VkCommandBuffer)> recordFunction;
};

class ThreadObject {
public:
  std::vector<Object *> objectsInThread;
  size_t numOfObjectsInThread = 0;

  explicit ThreadObject(VkDevice *_device);

  ~ThreadObject() = default;

  void destroy();

  void configure_buffer(VkDevice _device, uint32_t queueNodeIndex, uint32_t imageIndex);

  void remove();

  VkCommandBuffer get_cmdBuffer(uint32_t elNum, uint32_t imageIndex);

  std::vector<VkCommandBuffer> get_buffers();

private:
  std::vector<VkCommandBuffer> commandBuffer;
  VkCommandPool commandPool = VK_NULL_HANDLE;
  VkDevice *device;
};

class PM_IO_VULKAN_EXPORT VKSky::CImpl
#ifdef QT_LIB_ENABLE
    : public QObject
{
    Q_OBJECT
#else
{
#endif

public:
  CImpl();

  ~CImpl();

  // Public functions
  void run();

  void initWindow();

  void version_init(toolConfig *app, toolConfig *eng);

  void set_camera_ptr(ObjCamera *aCamera);
#ifdef QT_LIB_ENABLE
    VkWidget *get_vk_widget(QWidget *parent = nullptr);

    VkWidget *vulkan_widget = nullptr;
#endif
  // Static callback on Close Window

  void set_new_window_size(int width, int height);

  static void quit_from_engine(vk_sky::VKSky *obj);

  bool stop_engine();

  void send_key_event(void *ev, int key, int type);

  void send_mouse_event(void *event);

  void send_cursor_event(void *event);

  void send_scroll_event(void *event);

  void first_initialization();

  //
  void addObjectsToThread();

  void addObjectToThread(Object *object);

  void clearThreadObject(uint32_t begin);

  void RemoveObject(Object *obj);

  void loadAssets(Object *obj);

  //        void addObject(Object *object, ObjectFlags flag = ObjectFlags::BASE_LOAD);

  Object *createObject(const pipelineObject &pipeline, ObjectFlags flag = ObjectFlags::BASE_LOAD);

  Screen getScreen();

  void waitForCurrentFrameComplete();

  // Управление движком
  bool viewUpdated = false;
  bool paused = false;
  float timer = 0.0f;
  // Multiplier for speeding up (or slowing down) the global timer
  float timerSpeed = 0.25f;
  bool prepared = false;
  bool resized = false;
  /** @brief Last frame time measured using a high performance timer (if available) */
  float frameTimer = 1.0f;
  bool done = false;
  gui::UIOverlay uiOverlay;
  bool recreate = false;

  std::string CURRENT_PWD = "";

  void do_magick();

  /**
   * @brief
   *
   */
  void implLoop();

  void prepareFence();

  void camera_update();

  void initial_update();

  void endRender();

  void prepareObjectType();

  void impl_flush() const;

  void endLoop() const;

  void recreateSwapChain();

  void update();

  bool ready_to_close() const;

  void visible_ui(bool flag);
  /**
  *
  */

  /**
 *
 */
  void prepareUI();

  /**
*
*/
  void renderUI();

  void updateOverlay();

  bool is_ui_enable() const;


private:
  /**
   * @brief Create a Command Pool
   *
   */
  void createCommandPool();

  /**
   * @brief Create a Uniform Buffers object
   *
   */
  void createUniformBuffers();

  /**
   * @brief Create a Descriptor Pool object
   *
   */
  void createDescriptorPool();

  /**
   * @brief Create a Descriptor Sets object
   *
   */
  void createDescriptorSets();

  /**
   * @brief Create a Command Buffers object
   *
   */
  void createCommandBuffers();

  /**
   * @brief Create a Descriptor Set Layout object
   *
   */
  void createDescriptorSetLayout();

  /**
   * @brief Create a Graphics Pipeline object
   *
   */
  void createGraphicsPipeline();

  /**
   * @brief Create a Sync Objects object
   *
   */
  void createSyncObjects();

  /**
   * @brief Load assets (objects/textures/shaders from file or buffers)
   */
  void LoadAssets();

  /**
   *   @brief especial function for another visual effects
   */
  void viewChanged();

  /**
   *   @brief make update mapped uniform buffer of objects to view in on place (no need memcpy after change buffer)
   */
  void updateBufferMapped();

  /**
   *  @brief special function for make another command logic for compute
   */
  void createAdditinalBuffer();

  /**
   * @brief create memory barrier
   */
  void acquireBarrier(VkCommandBuffer _buffer);

  /**
   *
   * @param release memory barrier
   */
  void releaseBarrier(VkCommandBuffer _buffer);

  /**
   *
   */
  void createUI();

  /**
   * @brief
   *
   * @param commandBuffer
   * @return
   */
  void drawUI(VkCommandBuffer commandBuffer);

protected:
  std::vector<Object *> std_objects, trn_objects, all_objects,compute_objects; ///!< Буфферы для хранения разных по типу объектов
  VulkanSwapChain VkSwapChain;
  VulkanDevice vDevice;
  Screen screen;
  std::vector<pipelineObject> draw_objects;
  toolConfig *appConfig{}, *engConfig{};
  std::thread magick_thread;
  bool enableUI = true;


#ifdef __linux__
    std::chrono::time_point<std::chrono::system_clock> tStart;
#elif defined(_MSC_VER) || defined(_WIN32)
  std::chrono::time_point<std::chrono::steady_clock> tStart;
#endif

  /**
   * @brief Function init Vulkan
   *
   */
  void initVulkan();

  /**
   *  @brief function create instance of OS window
   * */
  void createInstance();

  /**
   * @brief clean all objects that was added with function createObject
   *
   */
  void cleanup();

  // TODO: Change on GLFW FLAG
  /**
   * @brief
   *
   * @return true
   * @return false
   */
  static bool checkValidationLayerSupport();

  void buildCommandBuffer();

  void manageViewportDraw(unsigned int current_buff);

  void createPipelineCache();

  void destroyCommandBuffers() const;

  void setupDebugMessenger();

  static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                            const VkAllocationCallbacks *pAllocator);

  static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  void createSynchronizationPrimitives();

  static std::vector<const char *> getRequiredExtensions();

  // VkPipelineShaderStageCreateInfo LoadShader(const std::string &filename, VkShaderStageFlagBits stage);
  void configureAssetsBuffer();

  // Render
  void prepareRender();

  void submitFrame();

  //
  void prepareMultiThreadRender();

  void threadRenderFunction(uint32_t threadIndex, uint32_t object_index, uint32_t bufferCount,
                            VkCommandBufferInheritanceInfo inheritanceInfo);

  // Frame counter to display fps
  uint32_t frameCounter = 0;
  uint32_t lastFPS = 0;

#ifdef __linux__
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTimestamp, tPrevEnd;
#elif defined(_MSC_VER) || defined(_WIN32)
  std::chrono::time_point<std::chrono::steady_clock> lastTimestamp, tPrevEnd;
#endif
  std::vector<VkShaderModule> shaderModules;
  std::vector<std::string> vertex_paths;
  std::vector<std::string> shader_paths;
  std::vector<std::string> tesc_paths;
  std::vector<std::string> tese_paths;
  std::vector<std::thread> threads;
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  std::vector<VkCommandBuffer> commandBuffers;
  std::vector<VkCommandBuffer> uiCmdBuffer;
  VkCommandBuffer primaryCommandBuffer{};

  uint32_t imageIndex = 0;
  // Thread Block
  vks::ThreadPool threadPool;
  uint32_t numThreads;
  uint32_t loadOnThread{}; //
  uint32_t threadObjectCount{};
  bool useQueryPool{};
  std::mutex mute;
  float color_array[4] = {0.0f, 0.0f, 0.0f, 1.0f};

  std::vector<const char *> enabledDeviceExtensions;
  std::vector<const char *> enabledInstanceExtensions;
  /** @brief Optional pNext structure for passing extension structures to device creation */
  void *deviceCreatepNextChain = nullptr;

  // for vulkan
  VkCommandPool cmdPool{};
  VkPipeline graphicsPipeline{};
  // Base classes
  VkDebugUtilsMessengerEXT debugMessenger{};

  VkPipelineCache pipelineCache{};

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;
  std::vector<VkFence> waitFences;
  VkFence renderFence = {};
  VkSubmitInfo submitInfo{};

  VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  size_t currentFrame = 0;

  // THREADS
  std::vector<cmdBufferRecThrdPrm> threads_parametrs;
  std::vector<ThreadObject> threadObjects;
  //
  ObjCamera *camera{};
  //
  bool m_signalFrame = false;


  struct UI_param {
    std::string ui_name = "UI";
    std::vector<std::string> preview_text = {u8"Some string for preview"};
    ImGuiWindowFlags ui_flags = ImGuiWindowFlags_AlwaysAutoResize;
    bool enable_fps_rate = true;

    struct {
      bool operator()() const {
        return fix;
      }

      ImVec2 pos() const {
        return vec;
      }

      ImGuiCond cond() const {
        return condition;
      }

      ImGuiCond condition = ImGuiCond_FirstUseEver;
      bool fix = true;
      ImVec2 vec = ImVec2(20, 20);
    } fix_on_position;
  } ui_param;
};

#endif // PK_DISPLAY_VK_DISPLAY_IMPL_H
