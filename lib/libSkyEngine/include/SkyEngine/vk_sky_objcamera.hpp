/*
* Basic camera class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#pragma once

#ifdef _MSC_VER
#define M_PI 3.141592265358979323864
#endif


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include <utility>
#include <memory>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class ObjCamera {
private:

  glm::vec3 up_vector = {0, -1, 0};
  glm::vec3 cam_direction{};
  glm::vec3 cam_pos_delta = glm::vec3(0, 0, 0);
  float cam_pitch{}, cam_scale{}, cam_head{};
  glm::vec3 mosse_position = glm::vec3(0, 0, 0);
  glm::vec3 look = {0, 0, 0};
  glm::vec3 new_up = {0, 0, 0};
  glm::vec3 new_right = {1, 0, 0};

  void updateViewMatrix();

  static glm::mat4 get_matrices(const float* matr);

  static glm::quat safeQuatLookAt(
      glm::vec3 const &position,
      glm::vec3 const &target,
      glm::vec3 const &up,
      glm::vec3 const &altUp);

  void changePitch(float degree);;

  void changeHeading(float degree);;


  glm::vec3 getRightVec() const;

  glm::vec3 getUp() const;

  glm::vec3 getViewDir() const;

  void updateViewMatrixQuat();

  static glm::mat4 getMatrixEulerRotate(glm::vec3 euler) {
    return glm::mat4();
  }

  glm::vec3 getRotationVector(glm::vec3 point_to, glm::vec3 point_from);

  glm::mat4 getRotationMatrix(glm::vec3 point_to, glm::vec3 point_from);

public:

  ObjCamera();
  ~ObjCamera();

  enum CameraType {
    FREE,
    WORLD,
    FIRST,
    THIRD,
    ORBITAL,
  };
  CameraType type = CameraType::FIRST;

  glm::vec3 rotation = glm::vec3();
  glm::vec3 buff_rotation = glm::vec3();
  glm::vec3 position = glm::vec3();
  glm::vec3 delta_position = glm::vec3();
  glm::vec4 viewPos = glm::vec4();
  //glm::vec3 base_zoomVec = glm::vec3(0,0,1);

  float rotationSpeed = 1.0f;
  float movementSpeed = 1.0f;
  float m_sensetivity = 0.0f;
  float m_sensetivity_koef = 0.002f;

  bool updated = false;
  bool flipY = false;
  bool change_target = false;
  bool cinematic = false;

  struct {
    glm::mat4 perspective;
    glm::mat4 view;
  } matrices{};

  struct {
    bool left = false;
    bool right = false;
    bool up = false;
    bool down = false;
  } keys;

  struct {
    bool up = false;
    bool down = false;
  } scroll;


  void updateAxDelta();

  void updateMovingTarget(glm::vec4 newPos, glm::vec3 *vel);

  float m_zoom = 0.0f;
  float fov = 45.0f;
  float znear{}, zfar{};
  glm::vec3 DeltaAxis = glm::vec3(0, 0, 500);
  glm::vec3 targetLook = glm::vec3(0, 0, 0);
  glm::vec3 targetVelocity = glm::vec3(0, 0, 0);
  float xAxsAngle = 0;
  float yAxsAngle = 0;
  float zAxsAngle = 0;
  float dxAxsAngle = 0;
  float dyAxsAngle = 0;
  float dzAxsAngle = 0;
  glm::vec4 *MovingTargetLook = nullptr;

  bool setZoom(float m_Zoom);

  bool moving() const;

  void moveCam(int x, int y);;

  float getNearClip();

  float getFarClip();

  void setPerspective(float fov, float aspect, float znear, float zfar);

  void setTargetDistance(float distance);

  void updateAspectRatio(float aspect);

  void setPosition(glm::vec3 position);

  void setTarget(glm::vec3 target);

  void setType(CameraType _type);

  void setRotation(glm::vec3 rotation);

  void rotate(glm::vec3 delta);

  void setTranslation(glm::vec3 translation);;

  void translate(glm::vec3 delta);

  void setRotationSpeed(float rotationSpeed);

  void setMovementSpeed(float movementSpeed);

  void update(float deltaTime);;

  // Update camera passing separate axis data (gamepad)
  // Returns true if view or position has been changed
  bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime);

  class Camera_CImpl;
  /////////////////////test
protected:
  std::unique_ptr<Camera_CImpl> u_ptr_camera;
};