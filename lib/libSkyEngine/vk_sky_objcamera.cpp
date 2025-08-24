//
// Created by SKYWORKER on 24.02.2025.
//
#include "Camera/camera_impl.h"
#include <SkyEngine/vk_sky_objcamera.hpp>
#include <memory>

void ObjCamera::updateViewMatrix() {

  //  u_ptr_camera->camera.minPitch = -glm::pi<float>()/2;
  //  u_ptr_camera->camera.maxPitch = glm::pi<float>()/2;

  auto *view = new float[16];
  camera_view_matrix(&u_ptr_camera->camera, view);
  matrices.view = get_matrices(view);
  delete[] view;
  viewPos = glm::vec4(position, 0.0f); // * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);

  updated = true;
}

glm::mat4 ObjCamera::get_matrices(const float *matr) {
  glm::mat4 mtr;
  mtr[0][0] = matr[0];
  mtr[0][1] = matr[1];
  mtr[0][2] = matr[2];
  mtr[0][3] = matr[3];

  mtr[1][0] = matr[4];
  mtr[1][1] = matr[5];
  mtr[1][2] = matr[6];
  mtr[1][3] = matr[7];

  mtr[2][0] = matr[8];
  mtr[2][1] = matr[9];
  mtr[2][2] = matr[10];
  mtr[2][3] = matr[11];

  mtr[3][0] = matr[12];
  mtr[3][1] = matr[13];
  mtr[3][2] = matr[14];
  mtr[3][3] = matr[15];
  return mtr;
}

void ObjCamera::update(float deltaTime) {
  updated = false;
  if (moving()) {
    float moveSpeed = deltaTime * movementSpeed;
    glm::vec3 delta = {0, 0, 0};

    glm::vec3 camFront;
    camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    camFront.y = sin(glm::radians(rotation.x));
    camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    camFront = glm::normalize(camFront);

    if (keys.up) {
      position += camFront * moveSpeed;
      delta.x = -moveSpeed;
    }
    if (keys.down) {
      position -= camFront * moveSpeed;
      delta.x = moveSpeed;
    }
    if (keys.left) {
      position -=
          glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) *
          moveSpeed;
      delta.z = -moveSpeed;
    }
    if (keys.right) {
      position +=
          glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) *
          moveSpeed;
      delta.z = moveSpeed;
    }

    camera_move(&u_ptr_camera->camera, {delta.x, delta.y, delta.z});
    updateViewMatrix();
  }
}

bool ObjCamera::updatePad(glm::vec2 axisLeft, glm::vec2 axisRight,
                          float deltaTime) {
  bool retVal = false;

  if (type == CameraType::FIRST) {
    // Use the common console thumbstick layout
    // Left = view, right = move

    const float deadZone = 0.0015f;
    const float range = 1.0f - deadZone;

    glm::vec3 camFront;
    camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
    camFront.y = sin(glm::radians(rotation.x));
    camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
    camFront = glm::normalize(camFront);

    float moveSpeed = deltaTime * movementSpeed * 2.0f;
    float rotSpeed = deltaTime * rotationSpeed * 50.0f;

    // Move
    if (fabsf(axisLeft.y) > deadZone) {
      float pos = (fabsf(axisLeft.y) - deadZone) / range;
      position =
          camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
      retVal = true;
    }
    if (fabsf(axisLeft.x) > deadZone) {
      float pos = (fabsf(axisLeft.x) - deadZone) / range;
      position =
          glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) *
          pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
      retVal = true;
    }

    // Rotate
    if (fabsf(axisRight.x) > deadZone) {
      float pos = (fabsf(axisRight.x) - deadZone) / range;
      rotation.y = pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
      retVal = true;
    }
    if (fabsf(axisRight.y) > deadZone) {
      float pos = (fabsf(axisRight.y) - deadZone) / range;
      rotation.x = pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
      retVal = true;
    }
  } else {
    // todo: move code from example base class for look-at
  }

  if (retVal) {
    updateViewMatrix();
  }

  return retVal;
}

glm::mat4 ObjCamera::getRotationMatrix(glm::vec3 point_to,
                                       glm::vec3 point_from) {
  glm::vec3 lookVector = point_to;
  if (lookVector == point_from)
    return glm::mat4(1.f);
  glm::vec3 direction = glm::normalize(lookVector - point_from);
  float dot = glm::dot(direction, up_vector);
  if (fabs(dot - (-1.0f)) < 0.000001f) {
    return glm::mat4(1.f);
  } else if (fabs(dot - (1.0f)) < 0.000001f) {
    auto quat = glm::quat();
    return glm::rotate(glm::mat4(1.f), glm::angle(quat), glm::axis(quat));
  }
  float dot_angle = acosf(dot); //
  auto cross = glm::normalize(glm::cross(up_vector, direction));
  auto rotate = glm::rotate(glm::mat4(1.f), dot_angle, cross);
  return rotate;
}

glm::vec3 ObjCamera::getRotationVector(glm::vec3 point_to,
                                       glm::vec3 point_from) {
  glm::vec3 lookVector = point_to;
  if (lookVector == point_from)
    return glm::vec3(0);
  glm::vec3 direction = glm::normalize(lookVector - point_from);
  float dot = glm::dot(direction, up_vector);
  if (fabs(dot - (-1.0f)) < 0.000001f) {
    return {1, 0, 0};
  } else if (fabs(dot - (1.0f)) < 0.000001f) {
    auto quat = glm::quat();
    return glm::axis(quat) * glm::angle(quat);
  }
  float dot_angle = acosf(dot); //
  auto cross = glm::normalize(glm::cross(up_vector, direction));
  auto rotate = cross * dot_angle;
  return rotate;
}

void ObjCamera::updateViewMatrixQuat() {
  glm::quat qPitch =
      glm::angleAxis(glm::radians(rotation.x), glm::vec3(1, 0, 0));
  glm::quat qYaw = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0, 1, 0));
  glm::quat qRoll =
      glm::angleAxis(glm::radians(rotation.z), glm::vec3(0, 0, 1));
  //

  auto m_d_orientaion = qPitch * qYaw;
  auto delta = glm::mix(glm::quat(0, 0, 0, 0), m_d_orientaion, rotationSpeed);

  glm::quat orientation = glm::normalize(qPitch * qYaw);
  auto rotate = glm::mat4_cast(orientation);

  // Save last rotation
  //        auto rotate_vector = getRotationVector(rotation,buff_rotation);
  //        auto quat = glm::quatLookAt(glm::normalize(rotation), glm::vec3(1,
  //        0, 0));
  auto translate_matrix = glm::translate(glm::mat4(1.0f), position);
  //        auto rotate_matrix =
  //        getRotationMatrix(glm::vec3(glm::radians(rotation)), glm::vec3(0));

  matrices.view = rotate * translate_matrix;
}

glm::vec3 ObjCamera::getViewDir() const {
  return -glm::transpose(matrices.view)[2];
}

glm::vec3 ObjCamera::getUp() const { return -glm::transpose(matrices.view)[1]; }

glm::vec3 ObjCamera::getRightVec() const {
  return glm::transpose(matrices.view)[0];
}

void ObjCamera::changeHeading(float degree) {
  if (degree < -5)
    degree = -5;
  else if (degree > 5)
    degree = 5;

  if ((cam_pitch > 90 && cam_pitch < 270) ||
      (cam_pitch < -90 && cam_pitch > -270))
    cam_head -= degree;
  else
    cam_head += degree;

  if (cam_head > 360.f)
    cam_head -= 360.f;
  else if (cam_head < -360)
    cam_head += 360.f;
}

void ObjCamera::changePitch(float degree) {
  if (degree < -5)
    degree = -5;
  else if (degree > 5)
    degree = 5;

  cam_pitch += degree;

  if (cam_pitch > 360.f)
    cam_pitch -= 360.f;
  else if (cam_pitch < -360)
    cam_pitch += 360.f;
}

glm::quat ObjCamera::safeQuatLookAt(const glm::vec3 &position,
                                    const glm::vec3 &target,
                                    const glm::vec3 &up,
                                    const glm::vec3 &altUp) {
  glm::vec3 direction = target - position;
  float dir_length = glm::length(direction);

  if (dir_length <= 0.001)
    return glm::quat(1, 0, 0, 0);
  direction /= dir_length;

  if (glm::abs(glm::dot(direction, up)) > .999f)
    return glm::quatLookAt(direction, altUp);
  else
    return glm::quatLookAt(direction, up);
}

ObjCamera::ObjCamera() { u_ptr_camera = std::make_unique<Camera_CImpl>(); }

bool ObjCamera::setZoom(float m_Zoom) {
  m_Zoom *= m_sensetivity;
  // if (glm::sign(targetLook.z - (position.z - (m_zoom - m_Zoom))) ==
  //     glm::sign(targetLook.z - (position.z - (m_zoom))))
  m_zoom -= m_Zoom;
  updateViewMatrix();
  return true;
}

bool ObjCamera::moving() const {
  if (dxAxsAngle != 0 || dyAxsAngle != 0 || dzAxsAngle != 0) {
    return true;
  }
  return keys.left || keys.right || keys.up || keys.down;
}

void ObjCamera::moveCam(int x, int y) {
  glm::vec3 mosse_delta = mosse_position - glm::vec3(x, y, 0);
  if (moving()) {
    changeHeading(.08f * mosse_delta.x);
    changePitch(.08f * mosse_delta.y);
  }
  mosse_position = glm::vec3(x, y, 0);
  updateViewMatrix();
}

void ObjCamera::setPerspective(float fov, float aspect, float znear,
                               float zfar) {
  this->fov = fov;
  this->znear = znear;
  this->zfar = zfar;
  matrices.perspective =
      glm::perspective(glm::radians(fov), aspect, znear, zfar);
  if (!flipY) {
    matrices.perspective[1][1] *= -1.0f;
  }
}

void ObjCamera::updateAspectRatio(float aspect) {
  matrices.perspective =
      glm::perspective(glm::radians(fov), aspect, znear, zfar);
  if (!flipY) {
    matrices.perspective[1][1] *= -1.0f;
  }
}

void ObjCamera::setPosition(glm::vec3 position) {
  this->position = position;
  camera_move(&u_ptr_camera->camera, {position.x, position.y, position.z});
  updateViewMatrix();
}

void ObjCamera::setTarget(glm::vec3 target) {
  this->targetLook = target;
  u_ptr_camera->camera.target_position = {target.x, target.y, target.z};
  updateViewMatrix();
}

void ObjCamera::setRotation(glm::vec3 rotation) {
  this->buff_rotation = this->rotation;
  this->rotation = rotation;
  updateViewMatrix();
}

void ObjCamera::rotate(glm::vec3 delta) {
  this->buff_rotation = this->rotation;
  this->rotation += delta;
  auto rad_del = glm::radians(delta);
  camera_rotate(&u_ptr_camera->camera, {rad_del.x, -rad_del.y, rad_del.z});
  updateViewMatrix();
}

void ObjCamera::setTranslation(glm::vec3 translation) {
  this->position = translation;
  updateViewMatrix();
}

void ObjCamera::translate(glm::vec3 delta) {
  this->position += delta;
  updateViewMatrix();
}

void ObjCamera::updateMovingTarget(glm::vec4 newPos, glm::vec3 *vel) {
  glm::vec3 bb = newPos;
  auto sub_vel = glm::vec3(0);
  if (vel != nullptr) {
    sub_vel = *vel;
  }
  position += bb - targetLook;

  auto cin_up = new_up;
  if (change_target) {
    new_up.y = 0;
    m_sensetivity = glm::distance(targetLook, position) * m_sensetivity_koef;
    change_target = false;
  }
  cin_up = glm::vec3(0, -1, 0);
  if (cinematic) {

    auto cin_right = 100.f * glm::normalize(glm::cross(cin_up, sub_vel));
    position = targetLook - cin_right * m_zoom * 0.01f; //-sub_vel*0.002f;
  }
  targetLook = bb;

  updateViewMatrix();
}

void ObjCamera::updateAxDelta() {
  DeltaAxis.x = position.x - targetLook.x;
  DeltaAxis.y = position.y - targetLook.y;
  DeltaAxis.z = position.z - targetLook.z;
}

float ObjCamera::getNearClip() { return znear; }

float ObjCamera::getFarClip() { return zfar; }

void ObjCamera::setRotationSpeed(float rotationSpeed) {
  this->rotationSpeed = rotationSpeed;
}

void ObjCamera::setMovementSpeed(float movementSpeed) {
  this->movementSpeed = movementSpeed;
}

void ObjCamera::setTargetDistance(float distance) {
  u_ptr_camera->camera.target_distance = distance;
}

void ObjCamera::setType(ObjCamera::CameraType _type) {
  switch (_type) {
  case CameraType::FIRST: {
    u_ptr_camera->camera.mode = CAMERA_MODE_FIRST_PERSON;
    break;
  }
  case CameraType::ORBITAL: {
    u_ptr_camera->camera.mode = CAMERA_MODE_ORBITAL;
    break;
  }
  case FREE:
    u_ptr_camera->camera.mode = CAMERA_MODE_FREE;
    break;
  case WORLD:
    u_ptr_camera->camera.mode = CAMERA_MODE_MOVE_IN_WORLDPLANE;
    break;
  case THIRD:
    u_ptr_camera->camera.mode = CAMERA_MODE_THIRD_PERSON;
    break;
  default:
    u_ptr_camera->camera.mode = CAMERA_MODE_FREE;
    break;
  }
}

ObjCamera::~ObjCamera() = default;

ObjCamera *ObjCamera::getApp(ObjCamera *ptr) {
  static ObjCamera *mPtr = nullptr;
  if (ptr)
    mPtr = ptr;
  return mPtr;
}
