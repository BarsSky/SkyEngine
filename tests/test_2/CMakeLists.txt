set(_TARGET_NAME test_2) # имя исполняемого файла

#добавление C и CPP файлов с учётом подкаталогов в проект
file(GLOB_RECURSE
    _SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

#добавление заголовочных файлов с учётом подкаталогов в проект
file(GLOB_RECURSE
    _HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)


set(_PRIVATE_INCLUDE_DIR # директории указанные в этой переменной видны в этой библиотеке,
    # а также в других библиотеках и исполняемых файлах, подключивших библиотеку
    #
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

# if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
#    target_compile_options(stxxl INTERFACE "/Zc:__cplusplus")
# endif()

#Препроцессоры устанавливаемые по умолчанию
set(_PRIVATE_DEFINITIONS
    #
    BOOST_ALL_NO_LIB
    BOOST_ERROR_CODE_HEADER_ONLY
    ELPP_NO_DEFAULT_LOG_FILE
    ELPP_THREAD_SAFE
)

if(MSVC)
    list(APPEND _PRIVATE_DEFINITIONS _MSVC_LANG=201710L NOMINMAX)
endif()

# создаёт проект для исполняемого файла
helper_add_executable(${_TARGET_NAME}
    _SOURCE_FILES
    _HEADER_FILES
    _PRIVATE_INCLUDE_DIR
    _PRIVATE_DEFINITIONS
    true
)

#Подключаемые (линкуемые) библиотеки 
#{		
set(_PUBLIC_LIBRARIS
    #

)

set(_PRIVATE_LIBRARIS
    #
    SKY::libSkyEngine
    Vulkan::Vulkan
)

if(QT_LIB_ENABLE)
    list(APPEND _PRIVATE_LIBRARIS Qt5::Widgets
        Qt5::Charts
        Qt5::Core)
endif()
#}
if(UNIX)
    list(APPEND _PUBLIC_LIBRARIS Qt5::X11Extras)
endif()

if(MINGW)
    list(APPEND _PUBLIC_LIBRARIS pthread)
endif()

#Линкует библиотеки
helper_target_link_libraries(${_TARGET_NAME}
    _PUBLIC_LIBRARIS
    _PRIVATE_LIBRARIS
)


#Здесь указываются необходимые для работы с исполняемым файлом 
install(DIRECTORY
    "${CMAKE_SOURCE_DIR}/data/fragments/shaders/comp/"
    DESTINATION "${SHADER_DIRECTORY}"
    COMPONENT applications #тип файла - настроечный
)
# install(DIRECTORY
# 		"${CMAKE_SOURCE_DIR}/data/models/"
# 		DESTINATION "${MODELS_DIRECTORY}"
# 		COMPONENT applications #тип файла - настроечный
# 		)
#install(FILES 
#		manual.doc #путь к файлу
#		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/doc"
#		COMPONENT docs #тип файла - документация
#		)		


