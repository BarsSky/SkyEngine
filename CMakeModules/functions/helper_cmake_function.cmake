
# Function for setting up base directory
function(install_default_directory 
			aPROJECT_NAME #	aPROJECT_NAME - Name of the project
			)
	
	if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
		set(CMAKE_INSTALL_PREFIX 
			"${CMAKE_BINARY_DIR}/binary" 
			CACHE PATH "Default installation Directory" FORCE)
	endif()

	set(${aPROJECT_NAME}_ROOT_DIR 
		${CMAKE_CURRENT_SOURCE_DIR} 
		CACHE PATH  "root directory")	
	
	set(${aPROJECT_NAME}_DEPENDENCIES_PATH 
		${${aPROJECT_NAME}_ROOT_DIR}/dependencies 
		CACHE PATH  "share dependencies")
		
	
	if(NOT CMAKE_DEBUG_POSTFIX)
		set(CMAKE_DEBUG_POSTFIX  
			"" 
			CACHE STRING "add a postfix, usually d on windows")
	endif()
	
	if(NOT CMAKE_RELEASE_POSTFIX)
		set(CMAKE_RELEASE_POSTFIX 
			"" 
			CACHE STRING "add a postfix, usually empty on windows")
	endif()
	
	set(${aPROJECT_NAME}_LIBRARIES 
		"" 
		CACHE STRING "Link these to use")
	

		
	set(CMAKE_INCLUDE_PATH 
		${CMAKE_INCLUDE_PATH} 
		CACHE STRING 
			"You may add additional search paths here.  
			Use ; to separate multiple paths.")
			
	set(CMAKE_LIBRARY_PATH 
		${CMAKE_LIBRARY_PATH} 
		CACHE STRING 
			"You may add additional search paths here. 
			Use ; to separate multiple paths.")
	
	list (FIND CMAKE_FIND_ROOT_PATH ${${aPROJECT_NAME}_DEPENDENCIES_PATH} _INDEX)
	if( (NOT DEFINED CMAKE_FIND_ROOT_PATH)#
		OR (_INDEX LESS 0)
		)
		set(CMAKE_FIND_ROOT_PATH 
			${CMAKE_FIND_ROOT_PATH} 
			${${aPROJECT_NAME}_DEPENDENCIES_PATH}
			CACHE STRING 
			"You may add additional search paths here. 
			Use ; to separate multiple paths."
			FORCE )
	endif()

	unset(CMAKE_INSTALL_FULL_BINDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SBINDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LIBEXECDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SYSCONFDIR CACHE)
	unset(CMAKE_INSTALL_FULL_SHAREDSTATEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LOCALSTATEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_LIBDIR CACHE)
	unset(CMAKE_INSTALL_FULL_INCLUDEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_OLDINCLUDEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DATAROOTDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DATADIR CACHE)
	unset(CMAKE_INSTALL_FULL_INFODIR CACHE)
	unset(CMAKE_INSTALL_FULL_LOCALEDIR CACHE)
	unset(CMAKE_INSTALL_FULL_MANDIR CACHE)
	unset(CMAKE_INSTALL_FULL_DOCDIR CACHE)

	include(GNUInstallDirs)

	set(CMAKE_INSTALL_FULL_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SBINDIR ${CMAKE_INSTALL_FULL_SBINDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LIBEXECDIR ${CMAKE_INSTALL_FULL_LIBEXECDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SYSCONFDIR ${CMAKE_INSTALL_FULL_SYSCONFDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_SHAREDSTATEDIR ${CMAKE_INSTALL_FULL_SHAREDSTATEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LOCALSTATEDIR ${CMAKE_INSTALL_FULL_LOCALSTATEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_INCLUDEDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_OLDINCLUDEDIR ${CMAKE_INSTALL_FULL_OLDINCLUDEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DATAROOTDIR ${CMAKE_INSTALL_FULL_DATAROOTDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_INFODIR ${CMAKE_INSTALL_FULL_INFODIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_LOCALEDIR ${CMAKE_INSTALL_FULL_LOCALEDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_MANDIR ${CMAKE_INSTALL_FULL_MANDIR}  CACHE PATH "The absolute path" FORCE)
	set(CMAKE_INSTALL_FULL_DOCDIR ${CMAKE_INSTALL_FULL_DOCDIR}  CACHE PATH "The absolute path" FORCE)

	if(NOT DEFINED CMAKE_PACKAGE_INSTALL_DIR)
		if(WIN32)
			set(CMAKE_PACKAGE_INSTALL_DIR
				"cmake"
				CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
				where cmake   is installed"
				)
		else()
			set(CMAKE_PACKAGE_INSTALL_DIR
				"share/cmake"
				CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
				where cmake   is installed"
				)
		endif()
	endif()

	if(NOT ${CMAKE_INSTALL_DOCDIR})
		set(CMAKE_INSTALL_DOCDIR
			"doc"
			CACHE PATH "The directory relative to CMAKE_PREFIX_PATH
			where documetation   is installed"
			FORCE
			)
	endif()


	mark_as_advanced (
		${aPROJECT_NAME}_LIBRARIES	
		${aPROJECT_NAME}_DEPENDENCIES_PATH
		CMAKE_DEBUG_POSTFIX
		CMAKE_RELEASE_POSTFIX
		CMAKE_INCLUDE_PATH
		CMAKE_LIBRARY_PATH
		CMAKE_FIND_ROOT_PATH
		CMAKE_PACKAGE_INSTALL_DIR
		CMAKE_INSTALL_DOCDIR
    )
	set(CMAKE_MODULE_PATH_HELPER
			"${CMAKE_MODULE_PATH}/functions/"
			CACHE INTERNAL "Path to search modules"
			FORCE
			)
	set(CMAKE_MODULE_PATH 
		"${CMAKE_MODULE_PATH}"
		"${CMAKE_MODULE_PATH}/find"
		"${CMAKE_MODULE_PATH}/functions"
		CACHE STRING "Path to search modules"
		FORCE
		)

	if ("${CMAKE_INSTALL_PREFIX}" STREQUAL ""
		)
		set(${aPROJECT_NAME}_INSTALL_PREFIX "" CACHE INTERNAL "" FORCE)
	else()
		set(${aPROJECT_NAME}_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/" CACHE INTERNAL "" FORCE)
	endif()

endfunction()

function(install_target_binary_impl
		 aTARGET_NAME # - Name of the library to create.
		 aIS_EXPORT
		 )
	get_target_property(_TYPE ${aTARGET_NAME} TYPE)

	set(_INSTALL_PREFIX	${${PROJECT_NAME}_INSTALL_PREFIX})
	
	if (DEFINED DIRECTORY_TYPE)

		set(_COMONENT_TYPE ${DIRECTORY_TYPE})

		if (NOT ${_COMONENT_TYPE} STREQUAL "tests" AND NOT ${_COMONENT_TYPE} STREQUAL "examples")
			message(FATAL_ERROR "Invalid type of component" ${_COMONENT_TYPE})
		endif ()
		

		if(aIS_EXPORT)
			install(TARGETS ${aTARGET_NAME}
				EXPORT ${aTARGET_NAME}-export
				LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
				ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
				RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMONENT_TYPE}
				)
		else()
			install(TARGETS ${aTARGET_NAME}
					LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
					ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMONENT_TYPE}
					RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMONENT_TYPE}
					)
		endif()
	else ()
		if("${_TYPE}" STREQUAL "EXECUTABLE")
			set(_COMP_LIB "applications")
			set(_COMP_RT "applications")
		else()
			set(_COMP_LIB "libraries")
			set(_COMP_RT "applications")
		endif()
		
		if(aIS_EXPORT)
			install(TARGETS ${aTARGET_NAME}
				EXPORT ${aTARGET_NAME}-export
				LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
				ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}"  COMPONENT ${_COMP_LIB}
				RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMP_RT}
				)
		else()
			install(TARGETS ${aTARGET_NAME}
					LIBRARY DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
					ARCHIVE DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}" COMPONENT ${_COMP_LIB}
					RUNTIME DESTINATION "${_INSTALL_PREFIX}${CMAKE_INSTALL_BINDIR}" COMPONENT ${_COMP_RT}
					)
		endif()
	endif ()

endfunction()

# Define _LIB_NAME library to be built 
#       _SOURCE_FILES_VAR2  
#       aHEADER_FILES_VAR   - the header file names.
#       aINSTALL_BIN        - TRUE if the lib should be installed
function (helper_add_library 
				aTARGET_NAME # - Name of the library to create.
				aSOURCE_FILES_VAR #  - the source file names.
				aHEADER_FILES_VAR # - the header file names.
				aPUBLIC_DIR # - public include dir
				aPRIVATE_DIR # - private include dir
				aPUBLIC_DEFINITIONS # - public DEFINITIONS
				aPRIVATE_DEFINITIONS # - private DEFINITIONS
				aINSTALL_BIN #- TRUE if the lib should be installed
		)


	if(NOT ${PROJECT_NAME}_NAMESPACE)
		set(${PROJECT_NAME}_NAMESPACE ${PROJECT_NAME})
	endif()

	string(TOUPPER ${aTARGET_NAME} 
			_TARGET_NAME_UPPER
		)
	
	set(${_TARGET_NAME_UPPER}_BUILD_STATIC_TOO
		false 
		CACHE BOOL "Build ${aTARGET_NAME} as static library too"
		)
	set(${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES
		false
		CACHE BOOL "Link ${aTARGET_NAME} with static dependecies"
		)

	if(NOT ${_TARGET_NAME_UPPER}_NO_DYNAMIC)
		add_library(${aTARGET_NAME}
					SHARED
					${${aSOURCE_FILES_VAR}}
					${${aHEADER_FILES_VAR}}
					)
#		if (UNIX)					
#			set_target_properties(${aTARGET_NAME} PROPERTIES
#		                      BUILD_RPATH "\$ORIGIN:../${CMAKE_INSTALL_LIBDIR}"
#		                      )
#		endif()		                      					
		add_library(${${PROJECT_NAME}_NAMESPACE}::${aTARGET_NAME}
					ALIAS
					${aTARGET_NAME})
	
		target_compile_definitions(${aTARGET_NAME}
							PRIVATE ${_TARGET_NAME_UPPER}_EXPORTS
							PRIVATE ${${aPRIVATE_DEFINITIONS}}
							
							PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFINITIONS}
							PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)

		foreach(_INCL ${${aPUBLIC_DIR}})
			target_include_directories (${aTARGET_NAME}
					PUBLIC $<BUILD_INTERFACE:${_INCL}>
					)
		endforeach()
		target_include_directories (${aTARGET_NAME}
								PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
								)

        target_include_directories (${aTARGET_NAME}
								PRIVATE ${${aPRIVATE_DIR}}
								)

		if(DEFINED ${_TARGET_NAME_UPPER}_TARGET_SOVERSION AND DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION)
			set_target_properties(${aTARGET_NAME} PROPERTIES
					VERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION}
					SOVERSION ${${_TARGET_NAME_UPPER}_TARGET_SOVERSION}
					)
		elseif (DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION_MAJOR AND DEFINED ${_TARGET_NAME_UPPER}_TARGET_VERSION)
			set_target_properties(${aTARGET_NAME} PROPERTIES
					VERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION}
					SOVERSION ${${_TARGET_NAME_UPPER}_TARGET_VERSION_MAJOR}
					)
		else()
			message(STATUS "${aTARGET_NAME}. Version is not set")
		endif()

	endif()

    if (${${_TARGET_NAME_UPPER}_BUILD_STATIC_TOO})
    
        add_library(${aTARGET_NAME}_Static 
        			STATIC 
        			${${aSOURCE_FILES_VAR}}
        			${${aHEADER_FILES_VAR}}
        			)

		add_library(${${PROJECT_NAME}_NAMESPACE}::${aTARGET_NAME}_Static
				ALIAS
				${aTARGET_NAME}_Static)
				
		target_compile_definitions(${aTARGET_NAME}_Static
		                            PRIVATE ${${aPRIVATE_DEFINITIONS}}
									PUBLIC ${_TARGET_NAME_UPPER}_STATIC
									PUBLIC ${${PROJECT_NAME}_PLATFORM_DEFINITIONS}
									PUBLIC ${${aPUBLIC_DEFINITIONS}}
							)
		
        foreach(_INCL ${${aPUBLIC_DIR}})
            target_include_directories (${aTARGET_NAME}_Static
                    PUBLIC $<BUILD_INTERFACE:${_INCL}>
                    )
        endforeach()

        target_include_directories (${aTARGET_NAME}_Static
                PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                )

		target_include_directories (${aTARGET_NAME}_Static  
								PRIVATE ${${aPRIVATE_DIR}}
								)			
    endif()	

    #INSTALLATION
    if (${aINSTALL_BIN})

		if(TARGET ${aTARGET_NAME} )
			install_target_binary_impl(${aTARGET_NAME} TRUE)
		endif()

        if (TARGET ${${aTARGET_NAME}_Static})
			install_target_binary_impl(${aTARGET_NAME}_Static FALSE)
        endif()


    endif()
	#${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/${aTARGET_NAME}    		
	set(${_TARGET_NAME_UPPER}_LIBRARIES
			${aTARGET_NAME}
			CACHE STRING "${aTARGET_NAME} libraries" FORCE)
	

	set(${_TARGET_NAME_UPPER}_INCLUDES
			${${aPUBLIC_DIR}}
			CACHE PATH  "Include path for the header files of ${aTARGET_NAME}" FORCE)
	
	set(${_TARGET_NAME_UPPER}_FOUND TRUE CACHE BOOL  "${aTARGET_NAME} found" FORCE)
	
	mark_as_advanced (
	${_TARGET_NAME_UPPER}_LIBRARIES
    ${_TARGET_NAME_UPPER}_INCLUDES
	#${aTARGET_NAME}_DEPENDENCIES_PATH
    )

endfunction()

# Add libs to a target
function (helper_target_link_libraries
			 aTARGET_NAME #Target name
			 aPUBLIC_LIBRARIES # Libraries for with target and target which is link with target
			 aPRIVATE_LIBRARIES # Libraries only for with target
			 )

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_NAME_UPPER
			)

	foreach(_LIB ${${aPUBLIC_LIBRARIES}})
		if(TARGET ${_LIB})
			set(_LIBRARY_NAME ${_LIB}#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
		
		
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
			elseif(TARGET ${_LIB})
				set(${_LIB}_BUILD_STATIC_TOO 
						true 
						CACHE BOOL "Build ${_LIB} as static library too"
						FORCE
					)
				message(FATAL_ERROR "For ${aTARGET_NAME} requirement static library ${_LIB}. Please reconfigure project")
			endif()
		endif()

		if(TARGET ${aTARGET_NAME})
			target_link_libraries(${aTARGET_NAME}  PUBLIC ${_LIBRARY_NAME})
		endif()
    	if (TARGET ${aTARGET_NAME}_Static)        	
        	target_link_libraries(${aTARGET_NAME}_Static PUBLIC ${_LIBRARY_NAME})
    	endif()    	
    endforeach()

	foreach(_LIB ${${aPRIVATE_LIBRARIES}})
		if(TARGET ${_LIB})
			set(_LIBRARY_NAME ${_LIB}#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
		else()
			set(_LIBRARY_NAME ${_LIB})
		endif()
				
		if(${${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES})
			if (TARGET ${_LIB}_Static)
				set(_LIBRARY_NAME ${_LIB}_Static#[[$<$<CONFIG:Debug>:${CMAKE_DEBUG_POSTFIX}>]])
			elseif(TARGET ${_LIB})
				set(${_LIB}_BUILD_STATIC_TOO 
						true 
						CACHE BOOL "Build ${_LIB} as static library too"
						FORCE
					)
				message(FATAL_ERROR "For ${aTARGET_NAME} requirement static library ${_LIB}. Please reconfigure project")
			endif()
		endif()

		if(TARGET ${aTARGET_NAME})
			target_link_libraries(${aTARGET_NAME}  PRIVATE ${_LIBRARY_NAME})
		endif()
    	if (TARGET ${aTARGET_NAME}_Static)        	
        	target_link_libraries(${aTARGET_NAME}_Static PRIVATE ${_LIBRARY_NAME})
    	endif()    	
    endforeach()
    
endfunction ()

# Add executable
function (helper_add_executable 
			aTARGET_NAME # - Name of the library to create.
			aSOURCE_FILES_VAR #  - the source file names.
			aHEADER_FILES_VAR # - the header file names.
			aINCLUDE_DIR # - include dir			
			aDEFINITIONS # - defenitions	
			aINSTALL_BIN #- TRUE if the lib should be installed
		  	#aCOMONENT_TYPE #- type of component (examples or tests)
			)
	string(TOUPPER ${aTARGET_NAME}
			_TARGET_NAME_UPPER
			)
	if(NOT ${PROJECT_NAME}_NAMESPACE)
		set(${PROJECT_NAME}_NAMESPACE ${PROJECT_NAME})
	endif()
    #Statically Linked
	set(${_TARGET_NAME_UPPER}_WITH_STATIC_DEPENDENCIES false CACHE
	BOOL "Link with static dependecies")
	
    add_executable(${aTARGET_NAME} ${${aSOURCE_FILES_VAR}} ${${aHEADER_FILES_VAR}})

	target_include_directories (${aTARGET_NAME}
								PRIVATE ${${aINCLUDE_DIR}}
								)
	
	target_compile_definitions(${aTARGET_NAME}
							PRIVATE ${${aDEFINITIONS}})
    #Install
	if (${aINSTALL_BIN})
		install_target_binary_impl(${aTARGET_NAME} FALSE)
	endif()
endfunction()

#Export library
function(helper_export_library
		aTARGET_NAME # - Name of the library to create.
		)

	string(TOUPPER ${aTARGET_NAME}
			_TARGET_UP
			)
			
	set(_OUT_DIRECTORY ${CMAKE_BINARY_DIR}/include/${aTARGET_NAME}/cmake)

	export(TARGETS ${aTARGET_NAME}
			FILE "${_OUT_DIRECTORY}/${aTARGET_NAME}Targets.cmake"
		   EXPORT_LINK_INTERFACE_LIBRARIES)

	set(CONF_TARGET_NAME ${aTARGET_NAME})
	set(CONF_LOOKING_FOR_FILES "config.h")

	configure_file(${CMAKE_MODULE_PATH_HELPER}/HelperFind.cmake
			"${_OUT_DIRECTORY}/Find${aTARGET_NAME}.cmake" @ONLY)

	include(CMakePackageConfigHelpers)

	if(NOT ${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH)
		set(${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH ${CMAKE_MODULE_PATH_HELPER}/Config.cmake.in)
	endif()

	if(WIN32)
		set(_CMAKE_PACKAGE_INSTALL_DIR
			${CMAKE_PACKAGE_INSTALL_DIR}
			)
	else()
		set(_CMAKE_PACKAGE_INSTALL_DIR
			${CMAKE_PACKAGE_INSTALL_DIR}/${aTARGET_NAME}
			)
	endif()

	configure_package_config_file( ${${_TARGET_UP}_CONFIG_CMAKE_FILE_PATH}
								  "${_OUT_DIRECTORY}/${aTARGET_NAME}Config.cmake"
								INSTALL_DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR}
								)

	set(_COMONENT_TYPE "cmake")
	if ( DEFINED DIRECTORY_TYPE)
		set(_COMONENT_TYPE ${DIRECTORY_TYPE})
	endif()


	install(FILES  "${_OUT_DIRECTORY}/${aTARGET_NAME}Config.cmake"
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)
	if( ${_TARGET_UP}_TARGET_VERSION)				
		set(_VERSION_CONFIG "${_OUT_DIRECTORY}/${aTARGET_NAME}ConfigVersion.cmake")
		
		write_basic_package_version_file(
			"${_VERSION_CONFIG}"
				VERSION "${${_TARGET_UP}_TARGET_VERSION}"
				COMPATIBILITY SameMajorVersion
		)
		
		install(FILES "${_VERSION_CONFIG}"
						DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
					)
	endif()

	install ( EXPORT ${aTARGET_NAME}-export
			FILE "${aTARGET_NAME}Targets.cmake"
			NAMESPACE ${${PROJECT_NAME}_NAMESPACE}::
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)

	install ( FILES
			"${_OUT_DIRECTORY}/Find${aTARGET_NAME}.cmake"
			DESTINATION ${_CMAKE_PACKAGE_INSTALL_DIR} COMPONENT ${_COMONENT_TYPE}
			)
	unset(CONF_LOOKING_FOR_FILES)
	unset(CONF_TARGET_NAME)
endfunction()

function(reset_global_variebles aPROJECT_NAME)
	set(${PROJECT_NAME}_PYTHONS_MODULES ""
	    CACHE INTERNAL "Python modules"
	    FORCE)
	set(${PROJECT_NAME}_MATLAB_MODULES ""
	    CACHE INTERNAL "Matlab modules"
	    FORCE)
	set(${PROJECT_NAME}_MATLAB_DATAS ""
	    CACHE INTERNAL "Matlab Data"
	    FORCE)
		
	set(${PROJECT_NAME}_DEB_RUNTIME_POSTINST
		"" 
		CACHE STRING ""
		FORCE
		)
	set(${PROJECT_NAME}_DEB_RUNTIME_PREINST
		"" 
		CACHE INTERNAL ""
		FORCE		
		)
	set(${PROJECT_NAME}_DEB_RUNTIME_POSTRM
		"" 
		CACHE INTERNAL ""
		FORCE
		)
	set(${PROJECT_NAME}_DEB_RUNTIME_PRERM
		"" 
		CACHE INTERNAL ""
		FORCE
		)
		
	set(CMAKE_MODULE_PATH
		"${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"		
		CACHE STRING "Path to search modules" FORCE)		 
	install_default_directory(${aPROJECT_NAME})
	
	unset(CPACK_DEBIAN_RUNTIME_PACKAGE_CONTROL_EXTRA)
	
endfunction()
# Configure project
macro(configure_project
		 aPROJECT_NAME #	aPROJECT_NAME - Name of the project
		 )

	reset_global_variebles(${aPROJECT_NAME})
	
	include (CMakeModules/functions/package_python.cmake)
	include (CMakeModules/functions/package_matlab.cmake)

	# make sure that the required libraries are
	# always found independent from LD_LIBRARY_PATH and the install
	# location
	if (UNIX)

		set(CMAKE_SKIP_BUILD_RPATH  FALSE)
		set(CMAKE_SKIP_RPATH  FALSE)
		set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
		#if (NOT DEFINED CMAKE_INSTALL_RPATH)
			list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
			if ("${isSystemDir}" STREQUAL "-1")
				set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
				file(RELATIVE_PATH _BIN_RELPATH
					 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}"
					 "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
				list(APPEND CMAKE_INSTALL_RPATH "\$ORIGIN/${_BIN_RELPATH}:../${CMAKE_INSTALL_LIBDIR}:${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
				#set(CMAKE_INSTALL_RPATH "${${PROJECT_NAME}_INSTALL_PREFIX}${CMAKE_INSTALL_LIBDIR}")
				set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
				set(CMAKE_SKIP_INSTALL_RPATH FALSE)
				message(STATUS " Setup RPATH to " "${CMAKE_INSTALL_RPATH}" )
			else()
				set(CMAKE_SKIP_INSTALL_RPATH TRUE)
			endif ()
		#endif ()
	endif ()

	# default to Release build (it's what most people will use)
	if (NOT CMAKE_BUILD_TYPE)
		set( CMAKE_BUILD_TYPE
			 Release
			 CACHE STRING
			 "Sets the configuration to build (Release, Debug, etc...)"
			 )
	endif()

	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/CMakeLists.txt")
		add_subdirectory(dependencies)
	endif()
	include (CMakeModules/dependencies_search.cmake)

	include (CMakeModules/configure.cmake)
	configure_file(CMakeModules/config.h.cmake 
				${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/config/config.h
				ESCAPE_QUOTES
				)

	install(FILES ${CMAKE_BINARY_DIR}/include/${PROJECT_NAME}/config/config.h
				DESTINATION "${${PROJECT_NAME}_INSTALL_PREFIX}include/${PROJECT_NAME}/config/" COMPONENT headers
		)

	include (CMakeModules/functions/configure_doxygen.cmake)
	include (CMakeModules/functions/configure_CPack.cmake)

endmacro()

macro(end_of_project
      aPROJECT_NAME #	aPROJECT_NAME - Name of the project
      )
	if(${PROJECT_NAME}_PYTHONS_MODULES)
		pacakage_python()
	endif()
	if(${PROJECT_NAME}_MATLAB_MODULES)
		pacakage_matlab()
	endif()

	generate_cpack()
endmacro()

macro(search_library_dependencies _PACKET _PACKET_DEPEND _TO_LIST)
	get_target_property(_LINK_LIBRARY ${_PACKET_DEPEND} INTERFACE_LINK_LIBRARIES)
	if(_LINK_LIBRARY)
		foreach(_LB ${_LINK_LIBRARY})
			#if(TARGET ${_LB} AND NOT ${_LB} MATCHES "(Python)$" )
			if(TARGET ${_LB})
				get_target_property(_IMPORTED_LIBRARY ${_LB} IMPORTED)
				get_target_property(_TYPE_LIBRARY ${_LB} TYPE)				
				if("${_TYPE_LIBRARY}" STREQUAL "SHARED_LIBRARY")
					if(NOT _IMPORTED_LIBRARY )
						list(FIND ${_TO_LIST} "${_LB}" _index)
						if(${_index} EQUAL -1)
							list(APPEND ${_TO_LIST} "${_LB}")
							if (UNIX)
								get_target_property(_ALIASED ${_LB} ALIASED_TARGET)
								if(TARGET ${_ALIASED})
									set_property(TARGET ${_ALIASED} APPEND PROPERTY
									             BUILD_RPATH ":\$ORIGIN:../${CMAKE_INSTALL_LIBDIR}"
									             )
								else()
									set_property(TARGET ${_LB} APPEND PROPERTY
									             BUILD_RPATH ":\$ORIGIN:../${CMAKE_INSTALL_LIBDIR}"
									             )
								endif()
							endif()
							search_library_dependencies(${_PACKET} ${_LB} ${_TO_LIST})
						endif()
					else()
						get_target_property(_LOCATION ${_LB} IMPORTED_LOCATION)
						if(_LOCATION)
							get_filename_component(_DIR_PATH ${_LOCATION} DIRECTORY)
							#get_filename_component(_FILE_NAME ${_LOCATION} NAME)
							file(RELATIVE_PATH _FILE_RELPATH
							     "${_DIR_PATH}/../../${PROJECT_NAME}/${_PACKET}"
							     "${_LOCATION}"
							     )
							get_filename_component(_DIR_RELPATH ${_FILE_RELPATH} DIRECTORY)
							if (UNIX)
								set_property(TARGET
								             ${_PACKET} APPEND PROPERTY
								             BUILD_RPATH ":${_DIR_RELPATH}"
								             )
							endif()
						endif()
					endif()
				endif()
			endif()
		endforeach()
	endif()
endmacro()

# Function
#
#      file_embed(_namespace _namespace _source _header_dir)
#
# reads binary file _source and creates a header file with .h extension
# and the name, combined from _namespace and _source basename
#(without path and extension) in the directory, given by _header_dir.

# In this created *.h file in _namespace namespace a single array
# static const char[] is defined with the name equal to _source basename.
# This array is initialised by the binary content of _source file

function(file_embed _namespace _source _header_dir)
	string(TOUPPER ${_namespace} _upper_namespace)
	get_filename_component(_name_source ${_source} NAME_WE)
	string(TOUPPER ${_name_source} _upper_name_source)
	string(CONCAT _ifdef_name ${_upper_namespace} "_" ${_upper_name_source})
	string(CONCAT _out_file_name ${_header_dir} "/" ${_namespace} "_" ${_name_source} ".h")
	file(READ ${_source} _content HEX)
	string(REGEX MATCHALL "([A-Fa-f0-9][A-Fa-f0-9])" _separated_hex ${_content})
	set(_cnt 0)
	foreach(hex IN LISTS _separated_hex)
		string(APPEND _out_text "'\\x${hex}',")
		math(EXPR _cnt "${_cnt}+1")
		if(_cnt GREATER 16)
			string(APPEND _out_text "\n\t\t")
			set(_cnt 0)
		endif()
	endforeach()
	set(_out_text "\
#ifndef ${_ifdef_name}\n\
#define ${_ifdef_name}\n\n\
namespace ${_namespace} { \n\
\tstatic const char ${_name_source}[] = {\n\
\t\t${_out_text} '\\x00'\
\t}\; \n\
}\n\n\
#endif //${_ifdef_name}\n" )
	message(STATUS -- out file: ${_out_file_name})
	file(WRITE ${_out_file_name} ${_out_text})
	set(FILE_EMBED_FILE_PATH ${_out_file_name} PARENT_SCOPE)
	set(FILE_EMBED_FILE_NAME ${_name_source} PARENT_SCOPE)
endfunction()

macro(helper_create_servicies)

	if(CPACK_GENERATOR STREQUAL "DEB")
	
		foreach(_SCRIPT preinst postinst postrem prerm)
			string(TOUPPER ${_SCRIPT}
				_SCRIPT_UP
				)			
			if ( ${PROJECT_NAME}_DEB_RUNTIME_${_SCRIPT_UP})								
				string(REGEX REPLACE ";" "\n" _LISTING "${${PROJECT_NAME}_DEB_RUNTIME_${_SCRIPT_UP}}")
				configure_file(
					${PROJECT_SOURCE_DIR}/CMakeModules/functions/services/${_SCRIPT}.sh.in
					${CMAKE_BINARY_DIR}/services/${_SCRIPT}
					@ONLY ESCAPE_QUOTES)
									
				file(COPY ${CMAKE_BINARY_DIR}/services/${_SCRIPT}
					DESTINATION 
					${CMAKE_BINARY_DIR}/runtime
					FILE_PERMISSIONS
					OWNER_READ OWNER_WRITE OWNER_EXECUTE 
					GROUP_READ GROUP_WRITE GROUP_EXECUTE
					WORLD_READ WORLD_WRITE WORLD_EXECUTE
					)
				list(APPEND CPACK_DEBIAN_RUNTIME_PACKAGE_CONTROL_EXTRA
					"${CMAKE_BINARY_DIR}/runtime/${_SCRIPT}")
			endif()	
		endforeach()		
	endif()
endmacro()
	
function(helper_create_service
		aTARGET_NAME # - Name of the applications.
		aARGS # - default start argument
		)
	
if(UNIX)
	set(PATH_TO_SYSTEMD "${CMAKE_INSTALL_PREFIX}/etc/systemd/system")
	set(PATH_TO_CONF "${CMAKE_INSTALL_PREFIX}/etc/default")

	
	configure_file(
		${PROJECT_SOURCE_DIR}/CMakeModules/functions/services/service.in
		${CMAKE_BINARY_DIR}/services/${aTARGET_NAME}.service
		@ONLY ESCAPE_QUOTES
	)

	install(FILES ${CMAKE_BINARY_DIR}/services/${aTARGET_NAME}.service
		DESTINATION "${PATH_TO_SYSTEMD}/"
		COMPONENT applications
	)
	configure_file(
		${PROJECT_SOURCE_DIR}/CMakeModules/functions/services/service.conf.in
		${CMAKE_BINARY_DIR}/services/${aTARGET_NAME}.conf
		@ONLY ESCAPE_QUOTES
	)
	install(FILES ${CMAKE_BINARY_DIR}/services/${aTARGET_NAME}.conf
		DESTINATION "${PATH_TO_CONF}/"
		COMPONENT applications
	)	
	set(${PROJECT_NAME}_DEB_RUNTIME_PREINST
		"${${PROJECT_NAME}_DEB_RUNTIME_PREINST}" 
		"systemctl is-active ${aTARGET_NAME}.service && sudo systemctl stop ${aTARGET_NAME}.service || true"
		CACHE STRING ""
		FORCE		
		)
	set(${PROJECT_NAME}_DEB_RUNTIME_POSTINST
		"${${PROJECT_NAME}_DEB_RUNTIME_POSTINST}"
		"systemctl daemon-reload || true"						
		CACHE STRING ""
		FORCE		
		)		
	set(${PROJECT_NAME}_DEB_RUNTIME_PRERM
		"${${PROJECT_NAME}_DEB_RUNTIME_PRERM}"
		"systemctl is-active ${aTARGET_NAME}.service && sudo systemctl stop ${aTARGET_NAME}.service"
		"systemctl disable ${aTARGET_NAME}"
		CACHE STRING ""
		FORCE		
		)
endif()				
endfunction()
