include(CheckSymbolExists)
include(CheckFunctionExists)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckCXXSourceCompiles)
include(CheckTypeSize)
include(FindPackageHandleStandardArgs)

message(STATUS "Configure project")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRD ON)

set(SHADER_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data/shaders"
        "" CACHE PATH "Shaders directory" FORCE)
set(MODELS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data/models"
        "" CACHE PATH "Models directory" FORCE)
set(DATA_DIRECTORY "${CMAKE_INSTALL_PREFIX}/data/"
        "" CACHE PATH "Models directory" FORCE)

set(LOGGING_DIRECTORY "${CMAKE_INSTALL_FULL_LOCALSTATEDIR}/log/${PROJECT_NAME}"
        "" CACHE PATH "Logging directory" FORCE)

option(VULKAN_VALIDATION_LAYERS "вывод лога вулкана" ON)

option(QT_LIB_ENABLE "Используются библиотеки QT для взаимодействия с ОС")

if(QT_LIB_ENABLE)
        set(CMAKE_AUTOMOC ON)
        set(CMAKE_AUTORCC ON)
        set(CMAKE_AUTOUIC ON)
endif()

option(GLFW_LIB_ENABLE "Используются библиотеки GLFW для взаимодействия с ОС" ON)

if(UNIX)
        option(VK_USE_PLATFORM_XCB_KHR "Включаем поддержку платформы протоколом XCB на линукс системах" ON)
elseif(WIN32)
        option(VK_USE_PLATFORM_WIN32_KHR "Включаем поддержку платформы Windows" ON)
endif()

option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy)
    if(CLANG_TIDY_EXECUTABLE)
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXECUTABLE}")
    else()
        message(WARNING "clang-tidy not found!")
    endif()
endif()