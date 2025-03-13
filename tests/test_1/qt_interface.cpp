//
// Created by ubuntu on 30.05.24.
//
#include "qt_interface.h"
#include <SkyEngine/config/config.h>


static glm::mat4 rotateAroundPoint(float rad, const glm::vec3 &point, const glm::vec3 &axis) {
  auto t1 = glm::translate(glm::mat4(1.0f), -point);
  auto r = glm::rotate(glm::mat4(1.0f), rad, axis);
  auto t2 = glm::translate(glm::mat4(1.0f), point);
  return t2 * r * t1;
}

QT_Interface::QT_Interface() : VKDisplay() {
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
  camera.type = Camera::lookat_v2;
//    camera.type  = Camera::CameraType::lookat;
  camera.flipY = false;
  camera.setPosition(glm::vec3(0.0f, 0.75f, 2.0f));
//    camera.setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
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
  modelAnimate = reinterpret_cast<GLTF_Model_Animate *>(createObject(
      pipelineObject(ePipelineObjectType::GLTF,
                     "../data/PathFinder/textured_mesh.glb")));//CesiumMan/glTF/CesiumMan.gltf")));
////////    modelAnimate->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
////////    Earth->load_textures_paths({_model_dir + "/earth_diff.ktx", _model_dir + "/earth_norm.ktx"});
  modelAnimate->load_object_shaders(
      {std::string(SHADER_DIRECTORY) + "mesh.frag.spv",std::string(SHADER_DIRECTORY) +"mesh.vert.spv"});

//    modelStatic = reinterpret_cast<GLTF_Model*>(createObject(
//            pipelineObject(ePipelineObjectType::GLTF,"../data/PathFinder/Pathfinder_1k.glb")));//CesiumMan/glTF/CesiumMan.gltf")));
//////    modelAnimate->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
//////    Earth->load_textures_paths({_model_dir + "/earth_diff.ktx", _model_dir + "/earth_norm.ktx"});
//    modelStatic->load_object_shaders({ SHADER_DIRECTORY"/mesh.frag.spv" ,SHADER_DIRECTORY"/mesh.vert.spv"});


  gpu_particle = reinterpret_cast<Partical_Model_GPU *>(createObject(
      pipelineObject(ePipelineObjectType::PARTICLE_GPU_OBJECT, "")
  ));
  gpu_particle->load_textures_paths(
      {std::string(MODELS_DIRECTORY) + "/particle_fire.ktx",});/// "../data/textures/particle_gradient_rgba.ktx"});
  gpu_particle->load_object_shaders({std::string(SHADER_DIRECTORY) + "/compute_particle.comp.spv",
                                     std::string(SHADER_DIRECTORY) + "/compute_particle.vert.spv",
                                     std::string(SHADER_DIRECTORY) + "/compute_particle.frag.spv"});

  //Axis
 Axis_X = reinterpret_cast<GLTF_Model *>(createObject(
     pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_x.gltf")));
 Axis_X->obj_position = axis_position;
 Axis_X->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});

 Axis_Y = reinterpret_cast<GLTF_Model *>(createObject(
     pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_y.gltf")));
 Axis_Y->obj_position = axis_position;
 Axis_Y->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});

 Axis_Z = reinterpret_cast<GLTF_Model *>(createObject(
     pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_z.gltf")));
 Axis_Z->obj_position = axis_position;
 Axis_Z->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});
}

void QT_Interface::PrepareBaseObjects() {
  //_model_dir = std::string("/binary/")+_model_dir;
//  Earth = reinterpret_cast<Model *>(createObject(
//      pipelineObject(ePipelineObjectType::OBJECT_3D, _model_dir + "/earth.obj")));
//  Earth->obj_position = glm::vec3(0.0f, 0.0f, 0.0f);
//  Earth->load_textures_paths({_model_dir + "/earth_diff.ktx", _model_dir + "/earth_norm.ktx"});
//  Earth->load_object_shaders({_shader_dir + "/earth.vert.spv", _shader_dir + "/earth.frag.spv"});
//  // положение луны
//  Moon = reinterpret_cast<Model *>(createObject(
//      pipelineObject(ePipelineObjectType::OBJECT_3D, _model_dir + "/moon.obj")));
//  Moon->obj_position = glm::vec3(0.0f, 0.0f, 6371.0f + 104400.f); // km 384400
//  Moon->load_textures_paths({_model_dir + "/moon_diff.ktx"});
//  Moon->load_object_shaders({_shader_dir + "/moon.vert.spv", _shader_dir + "/moon.frag.spv"});
//  // положение солнца
//  Sun = reinterpret_cast<Model *>(createObject(
//      pipelineObject(ePipelineObjectType::OBJECT_3D, _model_dir + "/sun.obj")));
//  Sun->obj_position = glm::vec3(0.0f, 0.0f, 149597.f); // 149'597'870'700f);//set km
//  Sun->load_textures_paths({_model_dir + "/Sun.png"});
//  Sun->load_object_shaders({_shader_dir + "/sun.vert.spv", _shader_dir + "/sun.frag.spv"});
  //Axis
//  Axis_X = reinterpret_cast<GLTF_Model *>(createObject(
//      pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_x.gltf")));
//  Axis_X->obj_position = axis_position;
//  Axis_X->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});

//  Axis_Y = reinterpret_cast<GLTF_Model *>(createObject(
//      pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_y.gltf")));
//  Axis_Y->obj_position = axis_position;
//  Axis_Y->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});

//  Axis_Z = reinterpret_cast<GLTF_Model *>(createObject(
//      pipelineObject(ePipelineObjectType::GLTF, _model_dir + "/axis_z.gltf")));
//  Axis_Z->obj_position = axis_position;
//  Axis_Z->load_object_shaders({_shader_dir + "/shader.vert.spv", _shader_dir + "/shader.frag.spv"});

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
  glm::vec3 nep = glm::vec3(1//*sin(glm::radians(timer_dog * 36.0f)) * 0.75f
                              ,2.0*timer_dog
                              ,10//*timer_dog
                              );
  //nep = glm::vec3(0,0,0);
  auto nep_tr = glm::vec4(nep,1);
    //camera.setPosition(glm::vec3(1,1,1));
    

  modelAnimate->gltf_animate_ubo.projection = camera.matrices.perspective;
  modelAnimate->gltf_animate_ubo.view = camera.matrices.view;
  modelAnimate->update(get_timer());
//    modelStatic->gltf_ubo.model = glm::mat4(1.f);
//    modelStatic->gltf_ubo.view = camera.matrices.view;
//    modelStatic->gltf_ubo.proj = camera.matrices.perspective;
//    modelStatic->gltf_ubo.lightPositon = glm::vec4(0.0,0,0,1);

  gpu_particle->ubo_pos_particle.emiter_position = nep;
  gpu_particle->ubo_pos_particle.model = glm::mat4(1.f);
  gpu_particle->ubo_pos_particle.projection = camera.matrices.perspective;
  gpu_particle->ubo_pos_particle.modelview = camera.matrices.view;
  gpu_particle->ubo_pos_particle.viewportDim = glm::vec2((float) *getScreen().width, (float) *getScreen().height);
  gpu_particle->ubo_gpu_particl.deltaT = /*paused ? 0.0f :*/ get_timer() * 20.5f;
  timer_dog += gpu_particle->ubo_gpu_particl.deltaT;

    //gpu_particle->ubo_gpu_particl.destX = 10*sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
    gpu_particle->ubo_gpu_particl.destY = 10*cos(glm::radians(timer_dog * 36.0f)) * 0.75f;
    gpu_particle->ubo_gpu_particl.destZ = 10*sin(glm::radians(timer_dog * 36.0f)) * 0.75f;
    gpu_particle->ubo_gpu_particl.estLifetime = 10;//*iter_dog;
    if(timer_dog>10){
      //timer_dog-=10;
      iter_dog++;
    }
  camera.updateMovingTarget(nep_tr,nullptr);
    // gpu_particle->ubo_pos_particle.emiter_position.x = 1*timer_dog;
    // gpu_particle->ubo_pos_particle.emiter_position.y = 0;
    // gpu_particle->ubo_pos_particle.emiter_position.z = 0;
  //gpu_particle->set_position(glm::vec3(timer_dog,0,0));

   Axis_X->gltf_ubo.model = glm::mat4(1.f);

  

   Axis_X->gltf_ubo.model = glm::translate(Axis_X->gltf_ubo.model,nep);
   Axis_X->gltf_ubo.model = glm::rotate(Axis_X->gltf_ubo.model, glm::radians(angle_x), xNorm_calc);
   Axis_X->gltf_ubo.model = glm::scale(Axis_X->gltf_ubo.model,glm::vec3(0.01,0.01,0.01));
   Axis_X->gltf_ubo.lightPositon = sun_position;
   Axis_X->gltf_ubo.view = camera.matrices.view;
   Axis_X->gltf_ubo.proj = camera.matrices.perspective;
   Axis_Y->gltf_ubo.model = glm::mat4(1.f);

   Axis_Y->gltf_ubo.model = glm::translate(Axis_Y->gltf_ubo.model,nep);
   Axis_Y->gltf_ubo.model = glm::rotate(Axis_Y->gltf_ubo.model, glm::radians(angle_y), yNorm_calc);
   Axis_Y->gltf_ubo.model = glm::scale(Axis_Y->gltf_ubo.model,glm::vec3(0.01,0.01,0.01));
   Axis_Y->gltf_ubo.lightPositon = sun_position;
   Axis_Y->gltf_ubo.view = camera.matrices.view;
   Axis_Y->gltf_ubo.proj = camera.matrices.perspective;
   //    memcpy(Axis_Y->uniformObjectBuffer.mapped, &Axis_Y->gltf_ubo, sizeof(Axis_Y->gltf_ubo));
   Axis_Z->gltf_ubo.model = glm::mat4(1.f);

   Axis_Z->gltf_ubo.model = glm::translate(Axis_Z->gltf_ubo.model,nep);

   Axis_Z->gltf_ubo.model = glm::rotate(Axis_Z->gltf_ubo.model, glm::radians(angle_z), zNorm_calc);
   Axis_Z->gltf_ubo.model = glm::scale(Axis_Z->gltf_ubo.model,glm::vec3(0.01,0.01,0.01));
   Axis_Z->gltf_ubo.lightPositon = sun_position;
   Axis_Z->gltf_ubo.view = camera.matrices.view;
   Axis_Z->gltf_ubo.proj = camera.matrices.perspective;

    

}

void QT_Interface::clear() {
  check_zero_distance = false;
  clear_objects();
}


// #include "moc_qt_interface.cpp"
