#
#   This file is part of BlueFramework, a simple 3D engine.
#	Copyright (c) 2016-2017 Technical University of Munich
#	Chair of Computational Modeling and Simulation.
#
#   BlueFramework is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License Version 3
#   as published by the Free Software Foundation.
#
#   BlueFramework is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program. If not, see <http://www.gnu.org/licenses/>.
#

if(GLEW_AUTOMATIC_INSTALL AND GLEW_INSTALL_DIR)
	MESSAGE(STATUS "Installing glew...")
	execute_process(COMMAND "${PROJECT_SOURCE_DIR}/external/Build_glew-2.0.0_Visual Studio 14 2015 Win64.cmd" ${GLEW_INSTALL_DIR} "${CMAKE_COMMAND}" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external)
	# find_path(GLEW_INCLUDE_DIR NAMES GL/glew.h PATHS ${GLEW_ROOT}/include)
	# set(GLEW_LIBRARY_DEBUG ${GLEW_ROOT}/build/lib/Debug/libglew32d.lib CACHE FILEPATH "Glew static debug library" FORCE)
	# set(GLEW_LIBRARY_RELEASE ${GLEW_ROOT}/build/lib/Release/libglew32.lib CACHE FILEPATH "Glew static release library" FORCE)
	# set(GLEW_LIBRARY_RELWITHDEBINFO ${GLEW_ROOT}/build/lib/RelWithDebInfo/libglew32.lib CACHE FILEPATH "Glew static release with debug info library" FORCE)
	set(GLEW_AUTOMATIC_INSTALL OFF CACHE BOOL "Automatically install glew" FORCE)
	# add_definitions(-DGLEW_STATIC)
endif()

