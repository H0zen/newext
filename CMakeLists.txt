# MaNGOS is a full featured server for World of Warcraft, supporting
# the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
#
# Copyright (C) 2005-2016  MaNGOS project <http://getmangos.eu>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

project(bfparser)

# CMake policies
cmake_minimum_required(VERSION 2.8)

# Until CMake 3.0 is the standard
# And a solution to set_directory_properties is found.
if(POLICY CMP0043)
    cmake_policy(SET CMP0043 OLD)
endif()

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${LIBS_DIR}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set the correct macro directory path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Define available cmake options below
# Hidden option to enable/disable PCH. DEV ONLY!
set(PCH ON)

find_package(Platform REQUIRED)
find_package(PCHSupport)

# Run out of build tree
set(CMAKE_BUILD_WITH_INSTALL_RPATH OFF)

include(cmake/ImportACE.cmake)

if(UNIX)
    find_package(ZLIB)
    find_package(BZip2)
endif()

#Include platform/compiler-specific definitions
include(${CMAKE_SOURCE_DIR}/cmake/SetDefinitions.cmake)

# Add dependency path
add_subdirectory(dep)

# Add source path
add_subdirectory(src)
