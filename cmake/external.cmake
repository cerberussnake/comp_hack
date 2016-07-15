# This file is part of COMP_hack.
#
# Copyright (C) 2010-2016 COMP_hack Team <compomega@tutanota.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Enable the ExternalProject CMake module.
INCLUDE(ExternalProject)

ExternalProject_Add(
    yamlcpp

    GIT_REPOSITORY https://github.com/comphack/yaml-cpp.git
    GIT_TAG comp_hack

    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/yaml-cpp
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> "-DCMAKE_CXX_FLAGS=-std=c++11 -stdlib=libc++"

    # Dump output to a log instead of the screen.
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON

    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libyaml-cpp.a
)

ExternalProject_Get_Property(yamlcpp INSTALL_DIR)

SET(YAML_INCLUDE "${INSTALL_DIR}/include")

ADD_LIBRARY(yaml-cpp STATIC IMPORTED)
ADD_DEPENDENCIES(yaml-cpp yamlcpp)
SET_TARGET_PROPERTIES(yaml-cpp PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libyaml-cpp.a")

ExternalProject_Add(
    googletest

    GIT_REPOSITORY https://github.com/google/googletest.git

    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/googletest
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> "-DCMAKE_CXX_FLAGS=-std=c++11 -stdlib=libc++"

    # Dump output to a log instead of the screen.
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON

    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libgtest.a <INSTALL_DIR>/lib/libgmock.a
        <INSTALL_DIR>/lib/libgmock_main.a
)

ExternalProject_Get_Property(googletest INSTALL_DIR)

SET(GTEST_INCLUDE_DIRS "${INSTALL_DIR}/include")

ADD_LIBRARY(gtest STATIC IMPORTED)
ADD_DEPENDENCIES(gtest googletest)
SET_TARGET_PROPERTIES(gtest PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libgtest.a")

ADD_LIBRARY(gmock STATIC IMPORTED)
ADD_DEPENDENCIES(gmock googletest)
SET_TARGET_PROPERTIES(gmock PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libgmock.a")

ADD_LIBRARY(gmock_main STATIC IMPORTED)
ADD_DEPENDENCIES(gmock_main googletest)
SET_TARGET_PROPERTIES(gmock_main PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libgmock_main.a")

SET(GMOCK_DIR "${INSTALL_DIR}")

# Set the name of the project.
PROJECT(comphack)

# Determine if the system is FreeBSD.
IF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    SET(BSD TRUE)
ENDIF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")

FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

SET(Boost_USE_STATIC_LIBS OFF)
FIND_PACKAGE(Boost "1.40" COMPONENTS thread system regex date_time
    program_options)

ExternalProject_Add(
    cucumbercpp

    GIT_REPOSITORY https://github.com/comphack/cucumber-cpp.git
    GIT_TAG comp_hack

    CMAKE_ARGS "-DGMOCK_DIR=${GMOCK_DIR}" -DCUKE_DISABLE_E2E_TESTS=ON
        -DCUKE_DISABLE_UNIT_TESTS=ON

    DEPENDS googletest

    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/cucumber-cpp
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR> "-DCMAKE_CXX_FLAGS=-std=c++11 -stdlib=libc++"

    # Dump output to a log instead of the screen.
    LOG_DOWNLOAD ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_INSTALL ON

    BUILD_BYPRODUCTS <INSTALL_DIR>/lib/libcucumber-cpp.a
        <INSTALL_DIR>/lib/libcucumber-cpp-nomain.a
)

ExternalProject_Get_Property(cucumbercpp INSTALL_DIR)

ADD_LIBRARY(cucumber-cpp STATIC IMPORTED)
ADD_DEPENDENCIES(cucumber-cpp cucumbercpp)
SET_TARGET_PROPERTIES(cucumber-cpp PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libcucumber-cpp.a")

ADD_LIBRARY(cucumber-cpp-nomain STATIC IMPORTED)
ADD_DEPENDENCIES(cucumber-cpp-nomain cucumbercpp)
SET_TARGET_PROPERTIES(cucumber-cpp-nomain PROPERTIES IMPORTED_LOCATION
    "${INSTALL_DIR}/lib/libcucumber-cpp-nomain.a")

SET(CUKE_INCLUDE_DIR "${INSTALL_DIR}/include" ${Boost_INCLUDE_DIRS})
SET(CUKE_LIBRARIES cucumber-cpp gmock gmock_main gtest
    ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT})
