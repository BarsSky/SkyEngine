//
// Created by f_f on 04.12.24.
//
#include <SkyEngine/vk_sky_device.hpp>
#include <SkyEngine/vk_sky_model.hpp>
#include <SkyEngine/vk_sky_pipelineobject.hpp>
#include <utility>


pipelineObject::pipelineObject(ePipelineObjectType type,
                               std::string object_path) {
  switch (type) {
  case ePipelineObjectType::OBJECT_3D:
    object = create_Model_3d(object_path);
    break;
  case ePipelineObjectType::OBJECT_2D:
    object = create_Model_2D();
    break;
  case ePipelineObjectType::HEIGHTMAP_3D:
    object = create_Model_3d(object_path, model_type::from_ktx_file);
    break;
  case ePipelineObjectType::LINE:
    object = create_Line();
    break;
  case ePipelineObjectType::GLTF:
    object = create_GLTF_Model(object_path);
    break;
  case ePipelineObjectType::GLTF_ANIMATE:
    object = create_GLTF_Model_Animate(object_path);
    break;
  case ePipelineObjectType::GLTF_SkyBox:
    object = create_GLTF_SkyBox(object_path);
    break;
  case ePipelineObjectType::TRANSPARENT_OBJECT_3D:
    object = create_Transparent_3d(object_path);
    break;
    // case ePipelineObjectType::TRANSPARENT_OBJECT_2D:
    //     break;
  case ePipelineObjectType::TERRIAN_OBJECT:
    object = create_Terrian(object_path);
    break;
  case ePipelineObjectType::PARTICLE_CPU_OBJECT:
    object = create_Particle_cpu();
    break;
  case ePipelineObjectType::PARTICLE_GPU_OBJECT:
    object = create_Particle_gpu();
    break;

  default:
    object = create_Model_3d(object_path);
  }
}

pipelineObject::pipelineObject(void *obj) : object(obj){ }

auto pipelineObject::create_Model_3d() -> void * {
  return object = static_cast<void *>(new Model3D());
}

auto pipelineObject::create_Model_3d(std::vector<Vertex> vertices,
                                      std::vector<uint32_t> indices) -> void * {
  return object = static_cast<void *>(new Model3D(vertices, indices));
}

auto pipelineObject::create_Model_3d(std::string object_path,
                                      model_type type) -> void * {
  return static_cast<void *>(new Model(std::move(object_path), type));
}

auto pipelineObject::create_Particle_cpu() -> void * {
  return object = static_cast<void *>(new Partical_Model_CPU());
}

pipelineObject::pipelineObject(ePipelineObjectType type,
                               const std::vector<Vertex> vertices,
                               std::vector<uint32_t> indices) {
  switch (type) {
  case ePipelineObjectType::OBJECT_3D:
    object = create_Model_3d(vertices, indices);
    break;
  case ePipelineObjectType::OBJECT_2D:
    object = create_Model_2D(vertices, indices);
    break;
  case ePipelineObjectType::LINE:
    object = create_Line(vertices);
    break;
  case ePipelineObjectType::TEXT_OVERLAY:
    object = create_TextOverlay();
    break;
  case ePipelineObjectType::UI_FORM:
    object = create_UI_Form();
    break;
  case ePipelineObjectType::TEXT_FORM:
    object = create_Text_Form();
    break;
  case ePipelineObjectType::SHAPE_FORM:
    object = create_Shape_Form();
    break;
  default:
    break;
  }
}

auto pipelineObject::create_Terrian(std::string object_path) -> void * {
  return object = static_cast<void *>(new Terrain_Model(
             std::move(object_path), model_type::from_ktx_file));
}

auto pipelineObject::create_Transparent_3d(std::string object_path) -> void * {
  return object = static_cast<void *>(new Transparent_Model(std::move(object_path)));
}

auto pipelineObject::create_GLTF_Model(std::string object_path) -> void * {
  return object = static_cast<void *>(new GLTF_Model(std::move(object_path)));
}

auto pipelineObject::create_GLTF_Model_Animate(std::string object_path) -> void * {
  return object = static_cast<void *>(new GLTF_Model_Animate(std::move(object_path)));
}

auto pipelineObject::create_GLTF_SkyBox(std::string object_path) -> void * {
  return object = static_cast<void *>(new GLTF_SkyBox(std::move(object_path)));
}

auto pipelineObject::create_Model_2D() -> void * {
  return object = static_cast<void *>(new Model2D());
}

auto pipelineObject::create_Model_2D(std::vector<Vertex> vertices,
                                      std::vector<uint32_t> indices) -> void * {
  return object = static_cast<void *>(new Model2D(vertices, indices));
}

auto pipelineObject::create_Line() -> void * {
  return object = static_cast<void *>(new Line());
}

auto pipelineObject::create_TextOverlay() -> void * {
  return object = static_cast<void *>(new TextOverlay());
}

auto pipelineObject::create_Line(std::vector<Vertex> vertices) -> void * {
  return object = static_cast<void *>(new Line(vertices));
}

auto pipelineObject::create_Particle_gpu() -> void * {
  return object = static_cast<void *>(new Partical_Model_GPU());
}

auto pipelineObject::create_UI_Form() -> void * {
  return object = static_cast<void *>(new UIForm());
}

auto pipelineObject::create_Text_Form() -> void * {
  return object = static_cast<void *>(new TextForm());
}

auto pipelineObject::create_Shape_Form() -> void * {
  return object = static_cast<void *>(new ShapeForm());
}