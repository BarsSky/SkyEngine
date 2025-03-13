#if(CPACK_GENERATOR STREQUAL "NSIS")
#endif()
if(CPACK_GENERATOR STREQUAL "DEB")	
	set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS
			"libxcb1,
			libvulkan1, 
			qt5-default, 
			libqt5charts5, 
			libqt5x11extras5")
	set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS
			"libxcb1-dev,
			qtbase5-dev, 
			libqt5charts5-dev,
			qt5-qmake, 
			qtbase5-dev-tools,			 
			libvulkan-dev,
			extra-cmake-modules,
			libqt5x11extras5-dev,
			libglfw3-dev,
			libglm-dev")
	
#	set(${PROJECT_NAME}_DEB_RUNTIME_PREINST
#		"${${PROJECT_NAME}_DEB_RUNTIME_PREINST}"
#		"sudo mkdir /var/log/${PROJECT_NAME} && sudo chmod 777 /var/log/${PROJECT_NAME}  || true"
#		CACHE STRING ""
#		FORCE
#		)
#	set(${PROJECT_NAME}_DEB_RUNTIME_PRERM
#		"${${PROJECT_NAME}_DEB_RUNTIME_PRERM}"
#		"rm -rf /var/log/${PROJECT_NAME}"
#		CACHE STRING ""
#		FORCE
#		)
endif()
