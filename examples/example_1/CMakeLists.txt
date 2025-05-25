set (_TARGET_NAME example_1) # имя исполняемого файла

#добавление C и CPP файлов с учётом подкаталогов в проект
file (GLOB_RECURSE _SOURCE_FILES *.cpp *.c)

#добавление заголовочных файлов с учётом подкаталогов в проект
file (GLOB_RECURSE _HEADER_FILES *.hpp *.h)

#Перечень директорий в которых будут искаться заголовочные файлы			
set (_PRIVATE_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}
		)

#Препроцессоры устанавливаемые по умолчанию
set (_PRIVATE_DEFINITIONS
		#
		)

# создаёт проект для исполняемого файла
helper_add_executable(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_PRIVATE_INCLUDE_DIR
		_PRIVATE_DEFINITIONS
		true
		)

# Добавляет информацию об версии исполняемого файла. Версия  берётся из файла ChangeLog.txt
# (В ChangeLog.txt имя  должно обязательно совпадать с именем исполняемого файла указанным здесь, в этом примере - example_1)		
configure_version_for(${_TARGET_NAME})


#Подключаемые (линкуемые) библиотеки 
#{			
set (_PUBLIC_LIBRARIS
		#
		)

set (_PRIVATE_LIBRARIS
		#
		)
#}

#Линкует библиотеки
helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

#Здесь указываются необходимые для работы с примером
#{	 
#install(FILES 
#		config.cfg #путь к файлу
#		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/config_files"
#		COMPONENT config #тип файла - настроечный
#		)		
#install(FILES 
#		manual.doc #путь к файлу
#		DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/doc"
#		COMPONENT docs #тип файла - документация
#		)		
#}

