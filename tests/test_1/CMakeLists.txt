set(_TARGET_NAME test_1) # имя исполняемого файла

#добавление C и CPP файлов с учётом подкаталогов в проект
file(GLOB
    _SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

#добавление заголовочных файлов с учётом подкаталогов в проект
file(GLOB
    _HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)


set(_PRIVATE_INCLUDE_DIR # директории указанные в этой переменной видны в этой библиотеке,
    # а также в других библиотеках и исполняемых файлах, подключивших библиотеку
    #
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
)

#Препроцессоры устанавливаемые по умолчанию
set(_PRIVATE_DEFINITIONS
    #
    BOOST_ALL_NO_LIB
    BOOST_ERROR_CODE_HEADER_ONLY
    ELPP_NO_DEFAULT_LOG_FILE
    ELPP_THREAD_SAFE
)

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
    Qt5::Widgets
    Qt5::Charts
    Qt5::Core
    Vulkan::Vulkan
)
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

helper_dep_install_qt()
#Здесь указываются необходимые для работы с исполняемым файлом 
# install(DIRECTORY 
# 		"./data/shaders/"
# 		DESTINATION "${SHADER_DIRECTORY}"
# 		COMPONENT applications #тип файла - настроечный
# 		)
# install(DIRECTORY 
# 		"./data/models/"
# 		DESTINATION "${MODELS_DIRECTORY}"
# 		COMPONENT applications #тип файла - настроечный
# 		)			
#install(FILES 
#		manual.doc #путь к файлу
#		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/doc"
#		COMPONENT docs #тип файла - документация
#		)		


