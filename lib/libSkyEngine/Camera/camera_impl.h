//
// Created by SKYWORKER on 24.02.2025.
//

#ifndef SKYENGINE_CAMERA_IMPL_H
#define SKYENGINE_CAMERA_IMPL_H

#include <SkyEngine/vk_sky_objcamera.hpp>

#define CAMERA_IMPLEMENTATION

#include "camera.h"

class ObjCamera::Camera_CImpl {
public:
  Camera_CImpl() = default;
  ~Camera_CImpl() = default;
  Camera camera;
private:
};

#endif //SKYENGINE_CAMERA_IMPL_H
