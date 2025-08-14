//
// Created by ubuntu on 30.05.24.
//
#include "interface.h"
#include "vk_sky_model.hpp"
#include "vk_sky_pipelineobject.hpp"
#include <SkyEngine/config/config.h>
#include <iomanip>
#include <sstream>
#include <string>

static glm::mat4 rotateAroundPoint(float rad, const glm::vec3 &point,
                                   const glm::vec3 &axis) {
  auto t1 = glm::translate(glm::mat4(1.0f), -point);
  auto r = glm::rotate(glm::mat4(1.0f), rad, axis);
  auto t2 = glm::translate(glm::mat4(1.0f), point);
  return t2 * r * t1;
}

Interface::Interface() : VKSky() {
  // TODO: Change on another type storage
  //  Объявляем storage для удобства хранения данных
  // формируем класс имитатора данных
}

Interface::~Interface() { clear(); }

void Interface::version_init() {
  appConfig.name = "Client";
  appConfig.major_version = 0;
  appConfig.minor_version = 1;
  appConfig.patch_version = 1;
  engConfig.name = "Entropy Engine";
  engConfig.major_version = 0;
  engConfig.minor_version = 2;
  engConfig.patch_version = 0;
}

void Interface::set_first_camera() {
  camera.type = ObjCamera::WORLD;
  camera.targetLook = glm::vec3(0.0f, 0.0f, 0.0f);
  camera.setPosition(glm::vec3(87000.0f, 0.75f, 2.0f));
  //    camera.setTargetDistance(8700);
  camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
  camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
  camera.setPerspective(
      60.0f, (float)*current_window.width / (float)*current_window.height, 0.1f,
      10000.0f);
  camera.movementSpeed = 10000.f;
}

void Interface::render() {}

void Interface::PrepareAssets() {
  PrepareBaseObjects();
  // modelAnimate = reinterpret_cast<GLTF_Model*>(createObject(
  //         pipelineObject(ePipelineObjectType::GLTF,std::string(MODELS_DIRECTORY)
  //         + "/Dragon_2.5_For_Animations.glb")));
  // modelAnimate->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  // modelAnimate->load_object_shaders({ std::string(SHADER_DIRECTORY)
  // +"/shader.frag.spv" ,
  //   std::string(SHADER_DIRECTORY) +"/shader.vert.spv"});

//   tarrain = static_cast<Terrain_Model *>(createObject(pipelineObject(
//       ePipelineObjectType::TERRIAN_OBJECT,
//       std::string(MODELS_DIRECTORY) + "/terrain_heightmap_r16.ktx")));
//   tarrain->load_textures_paths(
//       {std::string(MODELS_DIRECTORY) + "/terrain_heightmap_r16.png",
//        std::string(MODELS_DIRECTORY) + "/terrain_texturearray_rgba.ktx"});
//   tarrain->load_object_shaders(
//       {std::string(SHADER_DIRECTORY) + "/terrain.vert.spv",
//        std::string(SHADER_DIRECTORY) + "/terrain.frag.spv",
//        std::string(SHADER_DIRECTORY) + "/terrain.tesc.spv",
//        std::string(SHADER_DIRECTORY) + "/terrain.tese.spv"});

  SpaceShip = reinterpret_cast<GLTF_Model *>(createObject(
      pipelineObject(ePipelineObjectType::GLTF,
                     std::string(MODELS_DIRECTORY) + "/ColonShip1.glb")));
  SpaceShip->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  SpaceShip->load_object_shaders(
      {std::string(SHADER_DIRECTORY) + "/shader.vert.spv",
       std::string(SHADER_DIRECTORY) + "/shader.frag.spv"});

  SpaceShip->set_mouse_ptr(&mouse_point);
  // auto screen = glm::vec2(*getScreen().uWidth, *getScreen().uHeight);
  // modelStatic->set_screen_ptr(&screen);//FIXME: DEPRECATED FUNCTION
  SpaceShip->manage_constant.selected_unique_ID = 1;

  for (int i = 1; i < 20; i++) {
    GLTF_Model *space = reinterpret_cast<GLTF_Model *>(createObject(
        pipelineObject(ePipelineObjectType::GLTF,
                       std::string(MODELS_DIRECTORY) + "/ColonShip1.glb")));
    space->obj_position = glm::vec3(i * 100, 0.0f, 0.0f);
    space->load_object_shaders(
        {std::string(SHADER_DIRECTORY) + "/shader.vert.spv",
         std::string(SHADER_DIRECTORY) + "/shader.frag.spv"});

    space->set_mouse_ptr(&mouse_point);
    // auto screen = glm::vec2(*getScreen().uWidth, *getScreen().uHeight);
    // modelStatic->set_screen_ptr(&screen);//FIXME: DEPRECATED FUNCTION
    space->manage_constant.selected_unique_ID = 2 + i;
    SpaceShips.emplace_back(space);
  }

  gpu_particle = reinterpret_cast<Partical_Model_GPU *>(createObject(
      pipelineObject(ePipelineObjectType::PARTICLE_GPU_OBJECT, "")));
  gpu_particle->load_textures_paths(
      {std::string(MODELS_DIRECTORY) + "/particle01_rgba.ktx",
       std::string(MODELS_DIRECTORY) + "/particle_gradient_rgba.ktx"});
  gpu_particle->load_object_shaders(
      {std::string(SHADER_DIRECTORY) + "/compute_particle.comp.spv",
       std::string(SHADER_DIRECTORY) + "/compute_particle.vert.spv",
       std::string(SHADER_DIRECTORY) + "/compute_particle.frag.spv"});

  gpu_particle1 = reinterpret_cast<Partical_Model_GPU *>(createObject(
      pipelineObject(ePipelineObjectType::PARTICLE_GPU_OBJECT, "")));
  gpu_particle1->load_textures_paths(
      {std::string(MODELS_DIRECTORY) + "/particle01_rgba.ktx",
       std::string(MODELS_DIRECTORY) + "/particle_gradient_rgba.ktx"});
  gpu_particle1->load_object_shaders(
      {std::string(SHADER_DIRECTORY) + "/compute_particle.comp.spv",
       std::string(SHADER_DIRECTORY) + "/compute_particle.vert.spv",
       std::string(SHADER_DIRECTORY) + "/compute_particle.frag.spv"});

  text_info = reinterpret_cast<TextOverlay *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_OVERLAY)));
  text_info->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});
  text_info->updateFrameSize(getScreen().uWidth, getScreen().uHeight);

  texture_size = 320 * 256 * 4;
  texture_extent_buffer[0].buffer = new uint8_t[texture_size];
  texture_buffer_current = &texture_extent_buffer[0];
  texture_extent_buffer[1].buffer = new uint8_t[texture_size];
  texture_buffer_next = &texture_extent_buffer[1];

  for (int wp = 0; wp < texture_size; wp += 4) {
    texture_buffer_current->buffer[wp] = static_cast<uint8_t>(101);
    texture_buffer_current->buffer[wp + 1] = static_cast<uint8_t>(101);
    texture_buffer_current->buffer[wp + 2] = static_cast<uint8_t>(201);
    texture_buffer_current->buffer[wp + 3] = static_cast<uint8_t>(150);
  }

  form = new UIForm();
  form->set_draw_str_point(200, 200);

  shapeFormAll = reinterpret_cast<ShapeForm *>(
      createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeFormAll->init(form);
  shapeFormAll->texture_data = texture_buffer_current->buffer;
  shapeFormAll->image.texture_depth = 1;
  shapeFormAll->image.texture_byte_count = 4;
  shapeFormAll->image.texture_width = 320;
  shapeFormAll->image.texture_height = 256;
  shapeFormAll->load_object_shaders(
      {SHADER_DIRECTORY "/shape.vert.spv", SHADER_DIRECTORY "/shape.frag.spv"});

  textForm = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm->init(form);
  textForm->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  shapeForm = reinterpret_cast<ShapeForm *>(
      createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeForm->init(textForm);
  shapeForm->texture_data = texture_buffer_current->buffer;
  shapeForm->image.texture_depth = 1;
  shapeForm->image.texture_byte_count = 4;
  shapeForm->image.texture_width = 320;
  shapeForm->image.texture_height = 256;
  shapeForm->load_object_shaders(
      {SHADER_DIRECTORY "/shape.vert.spv", SHADER_DIRECTORY "/shape.frag.spv"});

  textForm2 = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm2->init(textForm);
  textForm2->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  textForm3 = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm3->init(textForm2);
  textForm3->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  textForm5 = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm5->init(textForm2);
  textForm5->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  textForm4 = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm4->init(textForm3);
  textForm4->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  shapeForm3 = reinterpret_cast<ShapeForm *>(
      createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeForm3->init(textForm3);
  shapeForm3->texture_data = texture_buffer_current->buffer;
  shapeForm3->image.texture_depth = 1;
  shapeForm3->image.texture_byte_count = 4;
  shapeForm3->image.texture_width = 320;
  shapeForm3->image.texture_height = 256;
  shapeForm3->load_object_shaders(
      {SHADER_DIRECTORY "/shape.vert.spv", SHADER_DIRECTORY "/shape.frag.spv"});

  textFormS = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textFormS->init(form);
  textFormS->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});

  textFormS2 = reinterpret_cast<TextForm *>(
      createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textFormS2->init(form);
  textFormS2->load_object_shaders(
      {SHADER_DIRECTORY "/text.vert.spv", SHADER_DIRECTORY "/text.frag.spv"});
}

void Interface::PrepareBaseObjects() {
  Earth = reinterpret_cast<Model *>(createObject(pipelineObject(
      ePipelineObjectType::OBJECT_3D, _model_dir + "/earth.obj")));
  Earth->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  Earth->load_textures_paths(
      {_model_dir + "/earth_diff.ktx", _model_dir + "/earth_norm.ktx"});
  Earth->load_object_shaders(
      {_shader_dir + "/earth.vert.spv", _shader_dir + "/earth.frag.spv"});
  // // положение луны
  // Moon = reinterpret_cast<Model*>(createObject(
  //     pipelineObject(ePipelineObjectType::OBJECT_3D, _model_dir +
  //     "/moon.obj")));
  // Moon->obj_position = glm::vec3(0.0f, 0.0f, 6371.0f + 104400.f); // km
  // 384400 Moon->load_textures_paths({_model_dir + "/moon_diff.ktx"});
  // Moon->load_object_shaders({_shader_dir + "/moon.vert.spv", _shader_dir +
  // "/moon.frag.spv"});
  // // положение солнца
  // Sun = reinterpret_cast<Model*>(createObject(
  //     pipelineObject(ePipelineObjectType::OBJECT_3D, _model_dir +
  //     "/sun.obj")));
  // Sun->obj_position = glm::vec3(0.0f, 0.0f, 149597.f); //
  // 149'597'870'700f);//set km Sun->load_textures_paths({_model_dir +
  // "/Sun.png"}); Sun->load_object_shaders({_shader_dir + "/sun.vert.spv",
  // _shader_dir + "/sun.frag.spv"}); Axis
  //    Axis_X = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_x.gltf")));
  //    Axis_X->obj_position = axis_position;
  //    Axis_X->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});
  //
  //    Axis_Y = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_y.gltf")));
  //    Axis_Y->obj_position = axis_position;
  //    Axis_Y->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});
  //
  //    Axis_Z = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_z.gltf")));
  //    Axis_Z->obj_position = axis_position;
  //    Axis_Z->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});
  //
  //    Axis_X2 = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_x.gltf")));
  //    Axis_X2->obj_position = axis_position;
  //    Axis_X2->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});
  //
  //    Axis_Y2 = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_y.gltf")));
  //    Axis_Y2->obj_position = axis_position;
  //    Axis_Y2->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});
  //
  //    Axis_Z2 = reinterpret_cast<GLTF_Model *>(createObject(
  //        pipelineObject(ePipelineObjectType::GLTF, _model_dir +
  //        "/axis_z.gltf")));
  //    Axis_Z2->obj_position = axis_position;
  //    Axis_Z2->load_object_shaders({_shader_dir + "/shader.vert.spv",
  //    _shader_dir + "/shader.frag.spv"});

  // Атмосфера
  // Atmosphere = reinterpret_cast<Transparent_Model *>(createObject(
  //   pipelineObject(ePipelineObjectType::TRANSPARENT_OBJECT_3D, _model_dir +
  //   "/clouds.obj")));
  // Atmosphere->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  // Atmosphere->load_textures_paths({_model_dir + "/clouds_ul.ktx"});
  // Atmosphere->load_object_shaders({_shader_dir + "/clouds.vert.spv",
  // _shader_dir + "/clouds.frag.spv"});

  // SPACE
  // SkyBox = reinterpret_cast<GLTF_SkyBox *>(createObject(
  //   pipelineObject(ePipelineObjectType::GLTF_SkyBox, _model_dir +
  //   "/skybox.gltf" /*"/skybox.gltf"*/)));
  // SkyBox->obj_position = glm::vec3(0.f, 0.f, 0.0f);
  // SkyBox->load_textures_paths({_model_dir + "/starcub_4k.ktx"
  // /*"/starcub_4k.ktx"*/}); SkyBox->load_object_shaders({
  //   _shader_dir + "/skybox.vert.spv" /*"/skybox.vert.spv"*/, _shader_dir +
  //   "/skybox.frag.spv" /*"/skybox.frag.spv"*/
  // });
}

void Interface::clear_objects() {}

void Interface::updateUniformBuffer() {
  camera.setPerspective(camera.fov,
                        ((float)*current_window.width) /
                            (float)*current_window.height,
                        0.1f, 100000000000.0f);

  sun_position =
      glm::vec4(0.0f, 0.0f, 149597.f, 0.0f); // 149'597'870'700f);//set km

//   tarrain->tesselation_ubo.projection = camera.matrices.perspective;
//   tarrain->tesselation_ubo.modelview = camera.matrices.view * glm::mat4(1.0f);
//   tarrain->tesselation_ubo.lightPos.y =
//       -0.5f - tarrain->tesselation_ubo.displacementFactor;
//   tarrain->tesselation_ubo.viewportDim =
//       glm::vec2((float)(*getScreen().uWidth), (float)(*getScreen().uHeight));

  Earth->object_ubo.model = glm::mat4(1.f);
  //        Earth->object_ubo.model =
  //        glm::scale(Earth->object_ubo.model,glm::vec3(1000,1000,1000));
  // Earth.ubo.model = rotateAroundPoint(time * glm::radians(0.2f),
  //                                                  Sun.obj_position,
  //                                                  glm::vec3(0.f, 1.f, 0.f))
  //                                                  *
  //                                glm::translate(Earth.ubo.model,
  //                                Earth.obj_position);
  //    Earth->object_ubo.model = glm::rotate(Earth->object_ubo.model,
  //                                          time * glm::radians(0.004f),
  //                                          glm::vec3(1.0f, 1.0f, 0.0f));

  Earth->object_ubo.lightPositon = sun_position;
  Earth->object_ubo.view = camera.matrices.view;
  Earth->object_ubo.proj = camera.matrices.perspective;
  Earth->object_ubo.viewPos = camera.viewPos;
  Earth->object_ubo.unique_id =
      glm::vec4{10, 1, 0, 0}; ///< Set base value of unique id

  // auto size_nodes_an = modelAnimate->getLinearNodesSize();
  // for (int i = 0; i < size_nodes_an; ++i) {
  //   modelAnimate->gltf_ubo.model = glm::mat4(1.f);
  //   modelAnimate->gltf_ubo.proj = camera.matrices.perspective;
  //   modelAnimate->gltf_ubo.view = camera.matrices.view;
  //   modelAnimate->gltf_ubo.viewPos = camera.viewPos;
  //   modelAnimate->gltf_ubo.lightPositon = sun_position;
  //   modelAnimate->updateUBO(&modelAnimate->gltf_ubo, i);
  //   modelAnimate->update(get_timer()*10);
  // }

  glm::vec3 new_center = {10, 0, 0};

  // SpaceShip->gltf_ubo.model = glm::translate(SpaceShip->gltf_ubo.model,
  // new_center); SpaceShip->gltf_ubo.model =
  // glm::rotate(SpaceShip->gltf_ubo.model, glm::radians(angle_x), xNorm);
  // SpaceShip->gltf_ubo.model = glm::scale(SpaceShip->gltf_ubo.model,
  // glm::vec3(0.1, 0.1, 0.1));
  auto size_nodes = SpaceShip->getLinearNodesSize();
  for (int i = 0; i < size_nodes; ++i) {
    SpaceShip->gltf_ubo.model = glm::mat4(1.f);
    SpaceShip->gltf_ubo.lightPositon = sun_position;
    SpaceShip->gltf_ubo.view = camera.matrices.view;
    SpaceShip->gltf_ubo.proj = camera.matrices.perspective;
    SpaceShip->gltf_ubo.viewPos = camera.viewPos;
    SpaceShip->gltf_ubo.unique_id =
        glm::vec4{1, i + 1, 0, 0}; ///< Set base value of unique id
    SpaceShip->updateUBO(&SpaceShip->gltf_ubo, i);
  }

  for (auto &ship : SpaceShips) {
    auto size_nodes = ship->getLinearNodesSize();
    auto &ubo = ship->gltf_ubo;
    glm::mat4 baseModel = glm::translate(glm::mat4(1.f), ship->obj_position);
    ubo.lightPositon = sun_position;
    ubo.view = camera.matrices.view;
    ubo.proj = camera.matrices.perspective;
    ubo.viewPos = camera.viewPos;
    for (int i = 0; i < size_nodes; ++i) {
      ubo.model = baseModel;
      ubo.unique_id =
          glm::vec4{1, i + 1, 0, 0}; /// TODO: Вынести назначение подобекта в
                                     /// момент формирования модели
      ship->updateUBO(&ship->gltf_ubo,
                      i); /// TODO: Учитывать позицию остальных объектов
                          /// относительно родительского
    }
  }

  //////// TextOverlay
  text_info->updateScale(1.0);
  text_info->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  text_info->beginTextUpdate();

  text_info->addText("title", 5.0f * 1, 5.0f * 1, TextOverlay::alignLeft);

  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << (get_timer() * 1000.0f) << "ms ("
     << fpsCounter << " fps)";
  text_info->addText(ss.str(), 5.0f * 1, 25.0f * 1, TextOverlay::alignLeft);

  text_info->addText("VideoCard", 5.0f * 1, 45.0f * 1, TextOverlay::alignLeft);

  // Display current model view matrix
  text_info->addText("model view matrix",
                     (float)(*getScreen().width) - 5.0f * 1, 5.0f * 1,
                     TextOverlay::alignRight);

  for (uint32_t i = 0; i < 4; i++) {
    ss.str("");
    ss << std::fixed << std::setprecision(2) << std::showpos;
    text_info->addText(ss.str(), (float)(*getScreen().width) - 5.0f * 1,
                       (25.0f + (float)i * 20.0f) * 1, TextOverlay::alignRight);
  }

  glm::vec3 projected = glm::project(
      glm::vec3(0.0f), SpaceShip->gltf_ubo.view, SpaceShip->gltf_ubo.proj,
      glm::vec4(0, 0, (float)(*getScreen().width),
                (float)(*getScreen().height)));
  text_info->addText("Selected object ID is " +
                         std::to_string(SpaceShip->idSelected()),
                     projected.x, projected.y, TextOverlay::alignCenter);

  text_info->endTextUpdate();

  // Atmosphere->trn_ubo.model = glm::mat4(1.f);
  // //    Atmosphere->trn_ubo.model = glm::scale(Atmosphere->trn_ubo.model,
  // glm::vec3(1000, 1000, 1000)); Atmosphere->trn_ubo.lightPositon =
  // sun_position;
  //
  // Atmosphere->trn_ubo.model = glm::rotate(Atmosphere->trn_ubo.model,
  //                                         glm::radians(-0.10f),
  //                                         glm::vec3(1.0f, 1.0f, 0.0f));
  //
  // Atmosphere->trn_ubo.modelview = camera.matrices.view;
  // Atmosphere->trn_ubo.projection = camera.matrices.perspective;

  // SkyBox->skybox_ubo.model = glm::mat4(1.f);
  // //        SkyBox->skybox_ubo.model =
  // glm::scale(SkyBox->skybox_ubo.model,glm::vec3(1000,1000,1000));
  // SkyBox->skybox_ubo.view = camera.matrices.view; // glm::lookAt(camEye,
  // camCenter, camUp); SkyBox->skybox_ubo.proj = camera.matrices.perspective;

  gpu_particle->ubo_pos_particle.emiter_position = glm::vec3(0, 0, 0);
  gpu_particle->ubo_pos_particle.model = glm::mat4(1.f);
  gpu_particle->ubo_pos_particle.projection = camera.matrices.perspective;
  gpu_particle->ubo_pos_particle.modelview = camera.matrices.view;
  gpu_particle->ubo_pos_particle.viewportDim =
      glm::vec2((float)*getScreen().width, (float)*getScreen().height);
  gpu_particle->ubo_gpu_particl.deltaT =
      /*paused ? 0.0f :*/ get_timer() * 20.5f;
  timer_dog += gpu_particle->ubo_gpu_particl.deltaT;

  // gpu_particle->ubo_gpu_particl.destX = 10*sin(glm::radians(timer_dog
  // * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.destY =
      10 * cos(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.destZ =
      10 * sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.estLifetime = 10; //*iter_dog;
  if (timer_dog > 10) {
    // timer_dog-=10;
    iter_dog++;
  }

  gpu_particle1->ubo_pos_particle.emiter_position = glm::vec3(10, 0, 0);
  gpu_particle1->ubo_pos_particle.model = glm::mat4(1.f);
  gpu_particle1->ubo_pos_particle.projection = camera.matrices.perspective;
  gpu_particle1->ubo_pos_particle.modelview = camera.matrices.view;
  gpu_particle1->ubo_pos_particle.viewportDim =
      glm::vec2((float)*getScreen().width, (float)*getScreen().height);
  gpu_particle1->ubo_gpu_particl.deltaT =
      /*paused ? 0.0f :*/ get_timer() * 20.5f;
  timer_dog += gpu_particle1->ubo_gpu_particl.deltaT;

  // gpu_particle->ubo_gpu_particl.destX = 10*sin(glm::radians(timer_dog
  // * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.destY =
      10 * cos(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.destZ =
      10 * sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.estLifetime = 10; //*iter_dog;
  if (timer_dog > 10) {
    // timer_dog-=10;
    iter_dog++;
  }

  textForm->updateScale(2.0);
  textForm->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textForm->beginTextUpdate();
  textForm->addText("New text ui check", TextForm::alignLeft);
  textForm->endTextUpdate();

  textForm2->setAlignRule(UIForm::AlignDown);
  textForm2->updateScale(2.0);
  textForm2->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textForm2->beginTextUpdate();
  textForm2->addText("Second line", TextForm::alignLeft);
  textForm2->endTextUpdate();

  textForm3->updateScale(2.0);
  textForm3->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textForm3->beginTextUpdate();
  textForm3->addText("Third line", TextForm::alignLeft);
  textForm3->endTextUpdate();

  textForm4->setAlignRule(UIForm::AlignDown);
  textForm4->updateScale(2.0);
  textForm4->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textForm4->beginTextUpdate();
  textForm4->addText("Fourth line", TextForm::alignLeft);
  textForm4->endTextUpdate();

  textForm5->setAlignRule(UIForm::AlignNext);
  textForm5->updateScale(2.0);
  textForm5->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textForm5->beginTextUpdate();
  textForm5->addText("Five line", TextForm::alignLeft);
  textForm5->endTextUpdate();

  textFormS->updateScale(2.0);
  textFormS->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textFormS->beginTextUpdate();
  textFormS->addText("We need Align", TextForm::alignLeft);
  textFormS->endTextUpdate();

  textFormS2->setAlignRule(UIForm::AlignDown);
  textFormS2->updateScale(2.0);
  textFormS2->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  textFormS2->beginTextUpdate();
  textFormS2->addText("We need Align 2", TextForm::alignLeft);
  textFormS2->endTextUpdate();

  // form->make_update();

  for (int wp = 0; wp < texture_size; wp += 4) {
    texture_buffer_current->buffer[wp] = static_cast<uint8_t>(101);
    texture_buffer_current->buffer[wp + 1] = static_cast<uint8_t>(101);
    texture_buffer_current->buffer[wp + 2] = static_cast<uint8_t>(201);
    texture_buffer_current->buffer[wp + 3] = static_cast<uint8_t>(165);
  }

  shapeForm->recreate_vertices();
  shapeForm->updateTexture(texture_buffer_current->buffer);

  for (int wp = 0; wp < texture_size; wp += 4) {
    texture_buffer_current->buffer[wp] = static_cast<uint8_t>(10);
    texture_buffer_current->buffer[wp + 1] = static_cast<uint8_t>(10);
    texture_buffer_current->buffer[wp + 2] = static_cast<uint8_t>(251);
    texture_buffer_current->buffer[wp + 3] = static_cast<uint8_t>(130);
  }

  shapeForm3->recreate_vertices();
  shapeForm3->updateTexture(texture_buffer_current->buffer);

  for (int wp = 0; wp < texture_size; wp += 4) {
    texture_buffer_current->buffer[wp] = static_cast<uint8_t>(191);
    texture_buffer_current->buffer[wp + 1] = static_cast<uint8_t>(181);
    texture_buffer_current->buffer[wp + 2] = static_cast<uint8_t>(201);
    texture_buffer_current->buffer[wp + 3] = static_cast<uint8_t>(145);
  }

  shapeFormAll->recreate_vertices();
  shapeFormAll->updateTexture(texture_buffer_current->buffer);

  recreateCommandBuffer();
}

void Interface::clear() {
  check_zero_distance = false;
  clear_objects();
}

void Interface::magickCursor(double x_pos, double y_pos) {
  mouse_point = glm::vec2(x_pos, y_pos);
  VKSky::magickCursor(x_pos, y_pos);
}

void Interface::OnUpdateUIOverlay(gui::UIOverlay *overlay) {

  overlay->header("Change camera speed");
  overlay->inputFloat("Speed value", &camera.movementSpeed, 10, 2);
}