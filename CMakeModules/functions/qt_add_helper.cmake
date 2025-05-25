function(helper_add_qt
        aPUBLIC_LIBRARIES
)
        if(WIN32)
                if(NOT QT_PATH)
                        set(QT_PATH "C:/Source/QT/5.15.2/mingw81_64/")
                endif()
                set(CMAKE_PREFIX_PATH ${QT_PATH})
                set(QT_LIBS_PATH ${QT_PATH}/lib/)
                set(QT_DLL_PATH ${QT_PATH}/bin/)
                message(STATUS ${QT_PATH})
        endif()

        find_package(Qt5Core REQUIRED)
        find_package(Qt5Widgets REQUIRED)
        find_package(Qt5Charts REQUIRED)
        if(USE_OpenGL)
                find_package(Qt5OpenGL REQUIRED)
                find_package(OpenGL REQUIRED)
                list(APPEND ${${aPUBLIC_LIBRARIES}} Qt5::OpenGL)
        endif()

        list(APPEND ${${aPUBLIC_LIBRARIES}} Qt5::Widgets Qt5::Charts Qt5::Core)
        message(STATUS "Added Qt libraries: " ${aPUBLIC_LIBRARIES})
endfunction()

function(helper_dep_install_qt)
        set(CMAKE_PREFIX_PATH ${QT_PATH})
        set(QT_LIBS_PATH ${QT_PATH}${QT_LIB_TYPE_PATH}/lib/)
        set(QT_DLL_PATH ${QT_PATH}${QT_LIB_TYPE_PATH}/bin/)
        if(WIN32)
                if(NOT Vulkan_FOUND)
                        set(QT_INSTALL_LIB ${QT_LIBS_PATH}/libQt5OpenGL.a)
                        set(QT_INSTQLL_DLL ${QT_DLL_PATH}/Qt5OpenGL.dll
                                ${QT_DLL_PATH}/opengl32sw.dll)
                endif()
                list(APPEND QT_INSTALL_LIB
                        ${QT_LIBS_PATH}/libQt5Core.a
                        ${QT_LIBS_PATH}/libQt5Charts.a
                        ${QT_LIBS_PATH}/libQt5Widgets.a
                        ${QT_LIBS_PATH}/libQt5Gui.a
                )

                list(APPEND QT_INSTQLL_DLL
                        ${QT_DLL_PATH}/Qt5Core.dll
                        ${QT_DLL_PATH}/Qt5Charts.dll
                        ${QT_DLL_PATH}/Qt5Widgets.dll
                        ${QT_DLL_PATH}/Qt5Gui.dll
                        ${QT_DLL_PATH}/libgcc_s_seh-1.dll
                        ${QT_DLL_PATH}/libGLESv2.dll
                        ${QT_DLL_PATH}/libstdc++-6.dll
                        ${QT_DLL_PATH}/libwinpthread-1.dll
                )

                install(
                        FILES
                        ${QT_INSTALL_LIB}
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
                        COMPONENT libraries)

                install(
                        FILES
                        ${QT_INSTQLL_DLL}
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
                        COMPONENT libraries)
                # QT DEPENDICES MAKE VARIATIONS
                install(
                        FILES
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/iconengines/qsvgicon.dll
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/iconengines/
                        COMPONENT source
                )
                install(
                        FILES
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/platforms/qwindows.dll
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/platforms/
                        COMPONENT source
                )
                install(
                        FILES
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qgif.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qicns.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qico.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qjpeg.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qsvg.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qtga.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qtiff.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qwbmp.dll
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/imageformats/qwebp.dll
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/imageformats/
                        COMPONENT source
                )
                install(
                        FILES
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/plugins/styles/qwindowsvistastyle.dll
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/styles/
                        COMPONENT source
                )
                install(
                        FILES
                        ${QT_PATH}${QT_LIB_TYPE_PATH}/translations/qt_en.qm
                        DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/translations/
                        COMPONENT source
                )
        endif()
endfunction()
