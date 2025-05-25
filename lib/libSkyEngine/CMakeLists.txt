set(_TARGET_NAME libSkyEngine)

file(GLOB _SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/ktx/*.c
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/stb/*.inl

)

if(GLFW_LIB_ENABLE)
        file(GLOB _SOURCE_FILES_A
                ${IMGUI_ROOT}/*.cpp
                ${IMGUI_ROOT}/misc/cpp/*.cpp
                ${IMGUI_ROOT}/backends/*.cpp)

        list(APPEND _SOURCE_FILES ${_SOURCE_FILES_A})
endif()

file(GLOB _HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/SkyEngine/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Camera/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/SkyEngine/qt_plugin/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/SkyEngine/qt_plugin/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/dirent/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/ktx/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/tinygltf/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/tinygltf/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/extension/tinyobjloader/*.h

)

if(GLFW_LIB_ENABLE)
        file(GLOB _HEADER_FILES_A
                ${IMGUI_ROOT}/*.h
                ${IMGUI_ROOT}/misc/cpp/*.h
                ${IMGUI_ROOT}/backends/*.h
                ${KHR_ROOT}/KHR/*.h)
        list(APPEND _HEADER_FILES ${_HEADER_FILES_A})
               
endif()

set(_PUBLIC_INCLUDE_DIR # директории указанные в этой переменной видны в этой библиотеке,
        # а также в других библиотеках и исполняемых файлах, подключивших библиотеку
        #
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME} #AUTO_MOC не умеет переходить по отноистельным путям
        ${IMGUI_ROOT}/
        ${IMGUI_ROOT}/misc/cpp/
        ${IMGUI_ROOT}/backends/
        ${KHR_ROOT}/KHR/
)

set(_PRIVATE_INCLUDE_DIR #
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Camera
)

set(_PUBLIC_DEFINITIONS #
)

set(_PRIVATE_DEFINITIONS
        BOOST_ERROR_CODE_HEADER_ONLY
        #
)

if(MSVC)
        list(APPEND _PRIVATE_DEFINITIONS _MSVC_LANG=201103L)
endif()

helper_add_library(
        ${_TARGET_NAME}
        _SOURCE_FILES
        _HEADER_FILES
        _PUBLIC_INCLUDE_DIR
        _PRIVATE_INCLUDE_DIR
        _PUBLIC_DEFINITIONS
        _PRIVATE_DEFINITIONS
        TRUE)

set(_PUBLIC_LIBRARIS
        ${Vulkan_LIBRARY}
)

if(QT_LIB_ENABLE)
        list(APPEND _PUBLIC_LIBRARIS
                Qt5::Widgets
                Qt5::Charts
                Qt5::Core)
        if(UNIX)
                list(APPEND _PUBLIC_LIBRARIS Qt5::X11Extras)
        endif()
endif()

if(MINGW)
        list(APPEND _PUBLIC_LIBRARIS pthread)
endif()


if(GLFW_LIB_ENABLE)
        list(APPEND _PUBLIC_LIBRARIS ${GLFW3_LIBRARY})
endif()

set(_PRIVATE_LIBRARIS #
)

helper_target_link_libraries(${_TARGET_NAME}
        _PUBLIC_LIBRARIS
        _PRIVATE_LIBRARIS
)

file(GLOB INSTALL_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/*.hpp)
install(
        FILES ${INSTALL_HEADER_FILES}
        DESTINATION
        "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
        COMPONENT headers)

file(GLOB INSTALL_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/qt_plugin/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/qt_plugin/*.hpp)
install(
        FILES ${INSTALL_HEADER_FILES}
        DESTINATION
        "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/qt_plugin"
        COMPONENT headers)

file(GLOB INSTALL_HEADER_FILES
        ${IMGUI_ROOT}/*.h
        ${IMGUI_ROOT}/misc/cpp/*.h
        ${IMGUI_ROOT}/backends/*.h
        ${KHR_ROOT}/KHR/*.h
)
install(
        FILES ${INSTALL_HEADER_FILES}
        DESTINATION
        "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}"
        COMPONENT headers)

file(GLOB INSTALL_HEADER_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/Camera/*.h)
install(
        FILES ${INSTALL_HEADER_FILES}
        DESTINATION
        "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/Camera/"
        COMPONENT headers)

if(QT_LIB_ENABLE)
        helper_dep_install_qt()
endif()

helper_export_library(${_TARGET_NAME})
