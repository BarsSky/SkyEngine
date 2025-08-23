cmake_minimum_required(VERSION 3.16.3)

project(SkyEngine C CXX)

# Значение переменных указаывается без спецсимволов
#{
set(${PROJECT_NAME}_ORGANIZATION "KDNK" CACHE STRING "Organization ")
set(${PROJECT_NAME}_AUTHORS "Nagaenko Konstantin" CACHE STRING "Authors")
set(${PROJECT_NAME}_SUPPORT_ADDRESS "knagaenko@mail.ru" CACHE
    STRING "Address to support")
set(${PROJECT_NAME}_NAMESPACE "SKY" CACHE STRING "Namespace of project")
set(${PROJECT_NAME}_LICENCE ${PROJECT_SOURCE_DIR}/LICENCE.txt CACHE PATH "Path to license")
set(${PROJECT_NAME}_VERSION "0.5" CACHE STRING "Version" FORCE)
#}

include(CMakeModules/functions/helper_cmake_function.cmake)

include(CMakeModules/functions/qt_add_helper.cmake)


configure_project(${PROJECT_NAME})

include(dependencies/JoltPhysics/Jolt/jolt.cmake)
################################################################################
# Sub directories
################################################################################
add_subdirectory(lib)
add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(tools)
end_of_project(${PROJECT_NAME})
