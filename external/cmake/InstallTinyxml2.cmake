find_path(TINYXML2_INSTALL_DIR OPTIONAL)
option(TINYXML2_AUTOMATIC_INSTALL OFF)
if(TINYXML2_AUTOMATIC_INSTALL AND TINYXML2_INSTALL_DIR)
	execute_process(COMMAND "${PROJECT_SOURCE_DIR}/external/Build_tinyxml2_Visual Studio 14 2015 Win64.cmd" ${TINYXML2_INSTALL_DIR} "${CMAKE_COMMAND}" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external)
	set(TINYXML2_ROOT ${TINYXML2_INSTALL_DIR}/tinyxml2-master CACHE PATH "Tinyxml2 root" FORCE)
	set(TINYXML2_AUTOMATIC_INSTALL OFF CACHE BOOL "Automatically install tinyxml2" FORCE)
endif()