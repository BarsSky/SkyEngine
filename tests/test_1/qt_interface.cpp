//
// Created by ubuntu on 30.05.24.
//
#include "qt_interface.h"
#include <SkyEngine/config/config.h>
#include <iomanip>


static glm::mat4 rotateAroundPoint(float rad, const glm::vec3 &point, const glm::vec3 &axis) {
  auto t1 = glm::translate(glm::mat4(1.0f), -point);
  auto r = glm::rotate(glm::mat4(1.0f), rad, axis);
  auto t2 = glm::translate(glm::mat4(1.0f), point);
  return t2 * r * t1;
}

QT_Interface::QT_Interface() : VKSky() {
  // TODO: Change on another type storage
  //  Объявляем storage для удобства хранения данных
  // формируем класс имитатора данных

}

QT_Interface::~QT_Interface() {

  clear();
}

void QT_Interface::version_init() {
  appConfig.name = "Client";
  appConfig.major_version = 0;
  appConfig.minor_version = 1;
  appConfig.patch_version = 1;
  engConfig.name = "Entropy Engine";
  engConfig.major_version = 0;
  engConfig.minor_version = 2;
  engConfig.patch_version = 0;
}

void QT_Interface::set_first_camera() {
  camera.type = ObjCamera::WORLD;
  camera.flipY = false;
  camera.setPosition(glm::vec3(0.0f, 0.75f, 2.0f));
  camera.setTarget(glm::vec3(0.0f, 0.0f, -0.5f));
  camera.setPerspective(60.0f, (float) *current_window.width / (float) *current_window.height, 0.1f, 10000.0f);
  camera.movementSpeed = 1.f;
}


void QT_Interface::render() {
  if (!attachToCursor) {
    if (animStart > 0.0f) {
      animStart -= get_timer() * 5.0f;
    } else if (animStart <= 0.0f) {
      timer += get_timer() * 0.04f;
      if (timer > 1.f)
        timer = 0.f;
    }
  }
}

void QT_Interface::PrepareAssets() {

  PrepareBaseObjects();

  SpaceShip = reinterpret_cast<GLTF_Model *>(createObject(
    pipelineObject(ePipelineObjectType::GLTF, std::string(MODELS_DIRECTORY) + "/ColonShip1.glb")));
  SpaceShip->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  SpaceShip->load_object_shaders({
    std::string(SHADER_DIRECTORY) + "/shader.vert.spv", std::string(SHADER_DIRECTORY) + "/shader.frag.spv"
  });

  SpaceShip->set_mouse_ptr(&mouse_point);
  SpaceShip->manage_constant.selected_unique_ID = 1;

  gpu_particle = reinterpret_cast<Partical_Model_GPU *>(createObject(
    pipelineObject(ePipelineObjectType::PARTICLE_GPU_OBJECT, "")
  ));
  gpu_particle->load_textures_paths(
    {
      std::string(MODELS_DIRECTORY) + "/particle01_rgba.ktx",
      std::string(MODELS_DIRECTORY) + "/particle_gradient_rgba.ktx"
    });
  gpu_particle->load_object_shaders({
    std::string(SHADER_DIRECTORY) + "/compute_particle.comp.spv",
    std::string(SHADER_DIRECTORY) + "/compute_particle.vert.spv",
    std::string(SHADER_DIRECTORY) + "/compute_particle.frag.spv"
  });

  gpu_particle1 = reinterpret_cast<Partical_Model_GPU *>(createObject(
    pipelineObject(ePipelineObjectType::PARTICLE_GPU_OBJECT, "")
  ));
  gpu_particle1->load_textures_paths(
    {
      std::string(MODELS_DIRECTORY) + "/particle01_rgba.ktx",
      std::string(MODELS_DIRECTORY) + "/particle_gradient_rgba.ktx"
    });
  gpu_particle1->load_object_shaders({
    std::string(SHADER_DIRECTORY) + "/compute_particle.comp.spv",
    std::string(SHADER_DIRECTORY) + "/compute_particle.vert.spv",
    std::string(SHADER_DIRECTORY) + "/compute_particle.frag.spv"
  });

  text_info = reinterpret_cast<TextOverlay *>(createObject(pipelineObject(ePipelineObjectType::TEXT_OVERLAY)));
  text_info->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});
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

  shapeFormAll = reinterpret_cast<ShapeForm *>(createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeFormAll->init(form);
  shapeFormAll->texture_data = texture_buffer_current->buffer;
  shapeFormAll->image.texture_depth = 1;
  shapeFormAll->image.texture_byte_count = 4;
  shapeFormAll->image.texture_width = 320;
  shapeFormAll->image.texture_height = 256;
  shapeFormAll->load_object_shaders({SHADER_DIRECTORY"/shape.vert.spv",SHADER_DIRECTORY"/shape.frag.spv"});


  textForm = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm->init(form);
  textForm->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});


  shapeForm = reinterpret_cast<ShapeForm *>(createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeForm->init(textForm);
  shapeForm->texture_data = texture_buffer_current->buffer;
  shapeForm->image.texture_depth = 1;
  shapeForm->image.texture_byte_count = 4;
  shapeForm->image.texture_width = 320;
  shapeForm->image.texture_height = 256;
  shapeForm->load_object_shaders({SHADER_DIRECTORY"/shape.vert.spv",SHADER_DIRECTORY"/shape.frag.spv"});

  textForm2 = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm2->init(textForm);
  textForm2->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});

  textForm3 = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm3->init(textForm2);
  textForm3->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});

  textForm5 = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm5->init(textForm2);
  textForm5->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});

  textForm4 = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textForm4->init(textForm3);
  textForm4->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});

  shapeForm3 = reinterpret_cast<ShapeForm *>(createObject(pipelineObject(ePipelineObjectType::SHAPE_FORM)));
  shapeForm3->init(textForm3);
  shapeForm3->texture_data = texture_buffer_current->buffer;
  shapeForm3->image.texture_depth = 1;
  shapeForm3->image.texture_byte_count = 4;
  shapeForm3->image.texture_width = 320;
  shapeForm3->image.texture_height = 256;
  shapeForm3->load_object_shaders({SHADER_DIRECTORY"/shape.vert.spv",SHADER_DIRECTORY"/shape.frag.spv"});


  textFormS = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textFormS->init(form);
  textFormS->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});

  textFormS2 = reinterpret_cast<TextForm *>(createObject(pipelineObject(ePipelineObjectType::TEXT_FORM)));
  textFormS2->init(form);
  textFormS2->load_object_shaders({SHADER_DIRECTORY"/text.vert.spv",SHADER_DIRECTORY"/text.frag.spv"});
}

void QT_Interface::PrepareBaseObjects() {
   // Атмосфера
  // Atmosphere = reinterpret_cast<Transparent_Model *>(createObject(
  //         pipelineObject(ePipelineObjectType::TRANSPARENT_OBJECT_3D, _model_dir + "/clouds.obj")));
  // Atmosphere->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
  // Atmosphere->load_textures_paths({_model_dir + "/clouds_ul.ktx"});
  // Atmosphere->load_object_shaders({_shader_dir + "/clouds.vert.spv", _shader_dir + "/clouds.frag.spv"});

  // SPACE
  SkyBox = reinterpret_cast<GLTF_SkyBox *>(createObject(
      pipelineObject(ePipelineObjectType::GLTF_SkyBox, _model_dir + "/skybox.gltf"/*"/skybox.gltf"*/)));
  SkyBox->obj_position = glm::vec3(0.f, 0.f, 0.0f);
  SkyBox->load_textures_paths({_model_dir + "/starcub_4k.ktx" /*"/starcub_4k.ktx"*/});
  SkyBox->load_object_shaders({
                                  _shader_dir + "/skybox.vert.spv" /*"/skybox.vert.spv"*/,
                                  _shader_dir + "/skybox.frag.spv" /*"/skybox.frag.spv"*/
                              });
}

void QT_Interface::clear_objects() {
}


void QT_Interface::updateUniformBuffer() {
 camera.setPerspective(camera.fov,
                        ((float) *current_window.width) / (float) *current_window.height,
                        0.1f, 100000000000.0f);

  sun_position = glm::vec4(0.0f, 0.0f, 149597.f, 0.0f); 

  glm::vec3 new_center = {10, 0, 0};


  auto size_nodes = SpaceShip->getLinearNodesSize();
  for (int i = 0; i < size_nodes; ++i) {
    SpaceShip->gltf_ubo.model = glm::mat4(1.f);
    SpaceShip->gltf_ubo.lightPositon = sun_position;
    SpaceShip->gltf_ubo.view = camera.matrices.view;
    SpaceShip->gltf_ubo.proj = camera.matrices.perspective;
    SpaceShip->gltf_ubo.viewPos = camera.viewPos;
    SpaceShip->gltf_ubo.unique_id = glm::vec4{1, i + 1, 0, 0}; ///< Set base value of unique id
    SpaceShip->updateUBO(&SpaceShip->gltf_ubo, i);
  }

  //////// TextOverlay
  text_info->updateScale(1.0);
  text_info->updateFrameSize(getScreen().uWidth, getScreen().uHeight);
  text_info->beginTextUpdate();

  text_info->addText("title", 5.0f * 1, 5.0f * 1, TextOverlay::alignLeft);

  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << (get_timer() * 1000.0f) << "ms (" << fpsCounter << " fps)";
  text_info->addText(ss.str(), 5.0f * 1, 25.0f * 1, TextOverlay::alignLeft);

  text_info->addText("VideoCard", 5.0f * 1, 45.0f * 1, TextOverlay::alignLeft);

  // Display current model view matrix
  text_info->addText("model view matrix", (float) (*getScreen().width) - 5.0f * 1, 5.0f * 1, TextOverlay::alignRight);

  for (uint32_t i = 0; i < 4; i++) {
    ss.str("");
    ss << std::fixed << std::setprecision(2) << std::showpos;
    text_info->addText(ss.str(), (float) (*getScreen().width) - 5.0f * 1, (25.0f + (float) i * 20.0f) * 1,
                       TextOverlay::alignRight);
  }

  glm::vec3 projected = glm::project(glm::vec3(0.0f),
                                     SpaceShip->gltf_ubo.view,
                                     SpaceShip->gltf_ubo.proj,
                                     glm::vec4(0, 0, (float) (*getScreen().width), (float) (*getScreen().height)));
  text_info->addText("Selected object ID is " + std::to_string(SpaceShip->idSelected()), projected.x, projected.y, TextOverlay::alignCenter);

  text_info->endTextUpdate();

  // Atmosphere->trn_ubo.model = glm::mat4(1.f);
  // //    Atmosphere->trn_ubo.model = glm::scale(Atmosphere->trn_ubo.model, glm::vec3(1000, 1000, 1000));
  // Atmosphere->trn_ubo.lightPositon = sun_position;
  //
  // Atmosphere->trn_ubo.model = glm::rotate(Atmosphere->trn_ubo.model,
  //                                         glm::radians(-0.10f),
  //                                         glm::vec3(1.0f, 1.0f, 0.0f));
  //
  // Atmosphere->trn_ubo.modelview = camera.matrices.view;
  // Atmosphere->trn_ubo.projection = camera.matrices.perspective;

  SkyBox->skybox_ubo.model = glm::mat4(1.f);
  //        SkyBox->skybox_ubo.model = glm::scale(SkyBox->skybox_ubo.model,glm::vec3(1000,1000,1000));
  SkyBox->skybox_ubo.view = camera.matrices.view; // glm::lookAt(camEye, camCenter, camUp);
  SkyBox->skybox_ubo.proj = camera.matrices.perspective;

  gpu_particle->ubo_pos_particle.emiter_position = glm::vec3(0, 0, 0);
  gpu_particle->ubo_pos_particle.model = glm::mat4(1.f);
  gpu_particle->ubo_pos_particle.projection = camera.matrices.perspective;
  gpu_particle->ubo_pos_particle.modelview = camera.matrices.view;
  gpu_particle->ubo_pos_particle.viewportDim = glm::vec2((float) *getScreen().width, (float) *getScreen().height);
  gpu_particle->ubo_gpu_particl.deltaT = /*paused ? 0.0f :*/ get_timer() * 20.5f;
  timer_dog += gpu_particle->ubo_gpu_particl.deltaT;

  //gpu_particle->ubo_gpu_particl.destX = 10*sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.destY = 10 * cos(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.destZ = 10 * sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle->ubo_gpu_particl.estLifetime = 10; //*iter_dog;
  if (timer_dog > 10) {
    //timer_dog-=10;
    iter_dog++;
  }

  gpu_particle1->ubo_pos_particle.emiter_position = glm::vec3(10, 0, 0);
  gpu_particle1->ubo_pos_particle.model = glm::mat4(1.f);
  gpu_particle1->ubo_pos_particle.projection = camera.matrices.perspective;
  gpu_particle1->ubo_pos_particle.modelview = camera.matrices.view;
  gpu_particle1->ubo_pos_particle.viewportDim = glm::vec2((float) *getScreen().width, (float) *getScreen().height);
  gpu_particle1->ubo_gpu_particl.deltaT = /*paused ? 0.0f :*/ get_timer() * 20.5f;
  timer_dog += gpu_particle1->ubo_gpu_particl.deltaT;

  //gpu_particle->ubo_gpu_particl.destX = 10*sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.destY = 10 * cos(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.destZ = 10 * sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
  gpu_particle1->ubo_gpu_particl.estLifetime = 10; //*iter_dog;
  if (timer_dog > 10) {
    //timer_dog-=10;
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

void QT_Interface::clear() {
  check_zero_distance = false;
  clear_objects();
}


// #include "moc_qt_interface.cpp"
