set(DIRECTORY_TYPE tests)

if (QT_LIB_ENABLE)
    add_subdirectory(test_1)
elseif (GLFW_LIB_ENABLE)
    add_subdirectory(test_2)
endif ()



