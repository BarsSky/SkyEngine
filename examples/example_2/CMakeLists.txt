set (_TARGET_NAME example_2)

#Looking for files in project		
file (GLOB_RECURSE _SOURCE_FILES *.cpp)
file (GLOB_RECURSE _HEADER_FILES *.hpp)

		
set (_PRIVATE_INCLUDE_DIR
		${CMAKE_CURRENT_SOURCE_DIR}
		)

set (_PRIVATE_DEFINITIONS
		#
		)

helper_add_executable(${_TARGET_NAME}
		_SOURCE_FILES
		_HEADER_FILES
		_PRIVATE_INCLUDE_DIR
		_PRIVATE_DEFINITIONS
		true
		)
		
configure_version_for(${_TARGET_NAME})

		
set (_PUBLIC_LIBRARIS
		#
		)

set (_PRIVATE_LIBRARIS
		#
		${${PROJECT_NAME}_NAMESPACE}::library_2
		)

helper_target_link_libraries(${_TARGET_NAME}
		_PUBLIC_LIBRARIS
		_PRIVATE_LIBRARIS
		)

