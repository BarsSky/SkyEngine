include(FindPackageHandleStandardArgs)

################################################################################
#Look for packages
################################################################################
if(UNIX)
    find_package(ECM REQUIRED NO_MODULE)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_FIND_MODULE_DIR})
endif()

if(QT_LIB_ENABLE)
    find_package(Qt5Core REQUIRED)
    find_package(Qt5Widgets REQUIRED)
    find_package(Qt5Charts REQUIRED)
    if(UNIX)
        find_package(Qt5X11Extras REQUIRED)
        find_package(XCB REQUIRED)
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/finds/")

find_package(GLFW3 REQUIRED)
if(NOT GLFW3_FOUND)
    message(WARNING "Not found glm library make shure you add GLFW3_ROOT path")
else()
    if(WIN32)
        message(STATUS "GLFW3 is " ${GLFW3_INCLUDE_DIR} " " ${GLFW3_LIBRARY})
        include_directories(${GLFW3_INCLUDE_DIR})
    endif()
endif()

find_package(GLM CONFIG REQUIRED)
if(NOT GLM_FOUND)
    message(WARNING "Not found glm library make shure you add GLM_ROOT path")
else()
    if(WIN32)
        include_directories(${GLM_ROOT})
    endif()
endif()

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(WARNING "Not found glm library make shure you add Vulkan_ROOT path")
else()
    if(WIN32)
        message(STATUS $ENV{VULKAN_SDK})
        include_directories($ENV{VULKAN_SDK}/include)
    endif()
endif()

set(LOGGER_ROOT ${${PROJECT_NAME}_DEPENDENCIES_PATH}/Logger)
set(IMGUI_ROOT ${${PROJECT_NAME}_DEPENDENCIES_PATH}/imgui)
