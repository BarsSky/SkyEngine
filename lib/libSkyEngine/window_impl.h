//
// Created by ubuntu on 31.07.24.
//

#ifndef PK_DISPLAY_WINDOW_IMPL_H
#define PK_DISPLAY_WINDOW_IMPL_H

#include <SkyEngine/vk_sky_device.hpp>
#include "window.hpp"

class VulkanDevice::Window_Impl{
public:
    Window_Impl(){

    }
    ~Window_Impl(){

    }
    MWindow crossWindow;
private:

};
#endif //PK_DISPLAY_WINDOW_IMPL_H
