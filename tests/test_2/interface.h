//
// Created by ubuntu on 25.05.24.
//

#ifndef ZUR_Interface_H
#define ZUR_Interface_H

#include <SkyEngine/ECSManager.hpp>
#include <SkyEngine/vk_sky.hpp>
#include <SkyEngine/vk_sky_model.hpp>
#include <SkyEngine/vk_sky_uioverlay.hpp>
#include <chrono>
#include <cstddef>
#include <thread>
#include <vector>

using namespace vk_sky;

struct GenerationTextureBuffer {
  ~GenerationTextureBuffer() {
    if (buffer != nullptr)
      delete[] buffer;
    buffer = nullptr;
  }

  uint8_t *getBuffer() const { return buffer; }

  uint8_t *buffer = nullptr;
};

class Interface : public VKSky {
public:
  // структура типа объектов
  enum { sun = 0, earth = 1, moon = 2 };

  // структура параметров объектов
  struct objParams {
    int type;
    glm::vec4 pos;     // позиция
    glm::vec3 vct_vel; // вектор движения
  };

  std::vector<objParams> elements;
  uint64_t TimeMark = 0;

  // Структура для отслеживания нажатий кнопок мыши
  struct {
    bool left = false;
    bool right = false;
    bool middle = false;
  } mouseButtons;

  Interface();

  ~Interface() override;

  void version_init() override;

  void set_first_camera() override;

  void render() override;

  void OnUpdateUIOverlay(gui::UIOverlay *overlay) override;

  glm::mat4 calc_rotation_matrix(glm::vec3 rot_vector);

  void PrepareAssets() override;

  void PrepareBaseObjects();

  // Формирование отображения объектов

  // Clear raid/scenary

  void updateUniformBuffer() override;

  void clear();

  void createObjPool();

  glm::vec3 getRotationVector(glm::vec3 point_to, glm::vec3 point_from,
                              bool counterclockwise, glm::vec3 rotate_axis);

  //    void magickKeyboard(QKeyEvent *event, int key, int type) override;
  //
  //    void magickScroll(QScrollEvent *event) override;
  //
  //    void magickMouse(QMouseEvent *event) override;
  //
  //    void magickCursor(QMouseEvent *event) override;

  /**
    @brief Определяем менеджер сущностей
  */
  ECSManager ecs_manager;

private:
  void magickCursor(double x_pos, double y_pos);
  // base directories
  const std::string _model_dir = MODELS_DIRECTORY;
  const std::string _shader_dir = SHADER_DIRECTORY;
  // итераторы объектов по отдельности
  std::vector<uint64_t> iter_draw;

  // количество объектов
  uint64_t draw_size = 0;

  // light options and obj positioning
  glm::vec4 sun_position;
  glm::vec3 point;
  glm::vec4 *currentTarget_obj;
  int current_camera_position = -1;
  // time
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::time_point<std::chrono::system_clock> currentTime;
  float time = 0;

  bool ready_to_draw = false;

  void clear_objects();

  //    QString current_log();
  bool check_zero_distance = false;
  bool isRaidClear = false;
  bool isRaidCreate = false;
  uint32_t raidCount = 9;
  // std::vector<FCO_imitation *> imit_lists;
  // bool view_3d = false;
  std::vector<std::string> clients_list;
  std::vector<uint64_t> uuid_list;

  std::string address = "127.0.0.1";
  int port = 3010;
  int trash_objects = 9;
  int active_search = -1;

  static bool connect() { return false; }

  /**
   *  Обзекты интерфейса
   *
   */
  // базовые объекты
  Model *Earth = nullptr;
  Model *Moon = nullptr;
  Model *Sun = nullptr;
  Terrain_Model *tarrain = nullptr;
  TextOverlay *text_info = nullptr;
  GLTF_Model *modelAnimate = nullptr;
  Partical_Model_GPU *gpu_particle = nullptr;
  Partical_Model_GPU *gpu_particle1 = nullptr;
  GLTF_Model *SpaceShip = nullptr;
  std::vector<GLTF_Model *> SpaceShips;
  GLTF_Model *Axis_X = nullptr, *Axis_Y = nullptr, *Axis_Z = nullptr,
             *Axis_X2 = nullptr, *Axis_Y2 = nullptr, *Axis_Z2 = nullptr;
  Transparent_Model *Atmosphere = nullptr;
  GLTF_SkyBox *SkyBox = nullptr;
  /// Текст и формы
  UIForm *form = nullptr;
  TextForm *textForm = nullptr;
  TextForm *textFormS = nullptr;
  TextForm *textFormS2 = nullptr;
  TextForm *textForm2 = nullptr;
  TextForm *textForm3 = nullptr;
  TextForm *textForm4 = nullptr;
  TextForm *textForm5 = nullptr;

  ShapeForm *shapeForm = nullptr;
  ShapeForm *shapeFormAll = nullptr;
  ShapeForm *shapeFormS = nullptr;
  ShapeForm *shapeFormS2 = nullptr;
  ShapeForm *shapeForm2 = nullptr;
  ShapeForm *shapeForm3 = nullptr;
  ShapeForm *shapeForm4 = nullptr;
  ShapeForm *shapeForm5 = nullptr;
  /// Текстура для форм
  uint32_t texture_size{};
  GenerationTextureBuffer *texture_buffer_current = nullptr;
  GenerationTextureBuffer *texture_buffer_next = nullptr;
  GenerationTextureBuffer texture_extent_buffer[2];
  // свалка моделируемых объектов
  Model *TestObject = nullptr;
  Partical_Model_CPU *Flameboost = nullptr;

  uint8_t *buffer = nullptr;
  uint64_t start_time_point{};
  uint64_t delta_to_time = 10 * uint64_t{1'000'000'000};
  glm::vec3 axis_position = {1, 1, 1};
  glm::vec3 xNorm = {1, 0, 0};
  glm::vec3 yNorm = {0, 1, 0};
  glm::vec3 zNorm = {0, 0, 1};
  float angle_x = 90, angle_y = 0, angle_z = 90;
  /// mute thread
  std::mutex mute, mute_clear;
  //    std::condition_variable wait_prepare;
  float timer_dog = 0;
  int iter_dog = 1;
  ///
  glm::vec2 mouse_point;
};

// #include "moc_Interface.cpp"

#endif // ZUR_Interface_H
