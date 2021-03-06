# SPDX-License-Identifier: MIT

project(pixel-terrain-super-build NONE)
cmake_minimum_required(VERSION 3.15)

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libpng OR
    NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib)
  message(FATAL_ERROR "You must execute project_root/third_party/prepare_libs.sh to initialize third-party libraries")
  return()
endif()

include(ExternalProject)

set(THIRD_PARTY_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/third_party)

add_custom_target(rclean
  COMMENT "Cleaning sub-projects..."
  COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR}/pixel-terrain --target clean
  COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR}/png/src/png-build --target clean
  COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR}/zlib/src/zlib-build --target clean
  COMMAND cmake --build ${CMAKE_CURRENT_BINARY_DIR} --target clean
  VERBATIM)

set_directory_properties(PROPERTIES
  ADDITIONAL_CLEAN_FILES third_party)

ExternalProject_Add(zlib
  PREFIX zlib
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/zlib
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_INSTALL_PREFIX}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})

ExternalProject_Add(png
  PREFIX png
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libpng
  CMAKE_ARGS
  -DCMAKE_INSTALL_PREFIX=${THIRD_PARTY_INSTALL_PREFIX}
  -DZLIB_CONFIG_CMAKE=${THIRD_PARTY_INSTALL_PREFIX}/share/cmake/zlib/zlib-config.cmake
  -DPNG_BUILD_ZLIB=OFF
  -DZLIB_ROOT=${THIRD_PARTY_INSTALL_PREFIX}
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  DEPENDS zlib)

ExternalProject_Add(pixel-terrain
  PREFIX pixel-terrain
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/pixel-terrain
  INSTALL_COMMAND ""
  CMAKE_ARGS
  -DZLIB_CONFIG_CMAKE=${THIRD_PARTY_INSTALL_PREFIX}/share/cmake/zlib/zlib-config.cmake
  -DPNG_CONFIG_CMAKE=${THIRD_PARTY_INSTALL_PREFIX}/lib/libpng/libpng16.cmake
  -DPIXEL_TERRAIN_SUPER_BUILDING=ON
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  DEPENDS zlib png)
