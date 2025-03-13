#include <SkyEngine/vk_sky_keyboard.hpp>

VulkanKeyboard *VulkanKeyboard::getApp(VulkanKeyboard *ptr)
{
    static VulkanKeyboard *mPtr = nullptr;
    if (ptr)
        mPtr = ptr;
    return mPtr;
}
#ifndef QT_LIB_ENABLE
void VulkanKeyboard::init_keyboard(GLFWwindow *window)
{
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_callback);
}

void VulkanKeyboard::key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    VulkanKeyboard::getApp()->core.add_state(new keyState(key, scancode, action, mode));
}

void VulkanKeyboard::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    VulkanKeyboard::getApp()->core.add_state(new scrollState(xoffset, yoffset));
}

void VulkanKeyboard::mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
    VulkanKeyboard::getApp()->core.add_state(new mouseState(button, action, mods));
}

void VulkanKeyboard::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    VulkanKeyboard::getApp()->core.add_state(new cursorState(xpos, ypos));
}
#endif