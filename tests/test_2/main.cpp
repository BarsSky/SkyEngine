#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include "interface.h"

int main()
{
/**
 * Add check if DEBUG
 */
#ifdef _DEBUG
#ifdef _WIN32
    system("../../../data/fragments/shaders/compile.bat");
#endif // WIN32

#ifdef __linux__
    system("chmod 777 ../../../data/fragments/shaders/compile.sh");
    system("sh ../../../data/fragments/shaders/compile.sh");
#endif // LINUX
#endif

    Interface app;
    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
