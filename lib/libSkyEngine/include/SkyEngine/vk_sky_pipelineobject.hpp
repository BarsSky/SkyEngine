//
// Created by ubuntu on 12.10.23.
//

#ifndef VKSky_PIPLINEOBJECT_H
#define VKSky_PIPLINEOBJECT_H

#include <SkyEngine/vk_sky_model.hpp>
#include <SkyEngine/vk_sky_object.hpp>

/*
 * We can add new pipelineObject to draw_objects
 * TODO: add functional:
 * remove added objects
 * or configure scenes
 */

enum class LIBSKYENGINE_EXPORT ePipelineObjectType : unsigned int {
  OBJECT_3D,
  OBJECT_2D,
  HEIGHTMAP_3D,
  LINE,
  GLTF,
  GLTF_ANIMATE,
  GLTF_SkyBox,
  TRANSPARENT_OBJECT_3D,
  TRANSPARENT_OBJECT_2D,
  FROM_BUFFER,
  TERRIAN_OBJECT,
  PARTICLE_CPU_OBJECT,
  PARTICLE_GPU_OBJECT,
  TEXT_OVERLAY,
  TEXT_FORM,
  SHAPE_FORM,
  UI_FORM
};

/**
 *  Класс помощник для более быстрого формирования объектов
 */
// TODO: Закрыть данный класс от пользователя, реализовывать формирование через
// штатный открытый API класса Magick
struct LIBSKYENGINE_EXPORT pipelineObject {
  explicit pipelineObject(
      ePipelineObjectType type,
      std::vector<Vertex> vertices = std::vector<Vertex>(),
      std::vector<uint32_t> indices = std::vector<uint32_t>());

  pipelineObject(ePipelineObjectType type, std::string object_path);

  explicit pipelineObject(void *obj);

  virtual ~pipelineObject() = default;

  auto create_Model_3d() -> void *;

  auto create_Model_3d(std::vector<Vertex> vertices,
                        std::vector<uint32_t> indices) -> void *;

  static auto create_Model_3d(std::string object_path,
                               model_type = model_type::from_obj_file) -> void *;

  // For particals
  auto create_Particle_cpu() -> void *;

  auto create_Particle_gpu() -> void *;

  auto create_Terrian(std::string object_path) -> void *;

  auto create_Transparent_3d(std::string object_path) -> void *;

  auto create_GLTF_Model(std::string object_path) -> void *;

  auto create_GLTF_Model_Animate(std::string object_path) -> void *;

  auto create_GLTF_SkyBox(std::string object_path) -> void *;

  auto create_Model_2D() -> void *;

  auto create_Model_2D(std::vector<Vertex> vertices,
                        std::vector<uint32_t> indices) -> void *;

  auto create_Line() -> void *;

  auto create_TextOverlay() -> void *;

  auto create_Line(std::vector<Vertex> vertices) -> void *;

  auto create_UI_Form() -> void *;

  auto create_Text_Form() -> void *;

  auto create_Shape_Form() -> void *;

  //
  //    uniformsBuffers ubo;
  void *object;
};

#endif // VKSky_PIPLINEOBJECT_H
