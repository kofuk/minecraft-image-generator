find_package(ZLIB)
if(NOT ZLIB_FOUND)
  include(ExternalProject)
  ExternalProject_Add(zlib_project
    URL https://zlib.net/zlib-1.2.11.tar.xz
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external -DCMAKE_BUILD_TYPE=Release)
  add_library(zlib IMPORTED STATIC)
  if(MSVC)
    set_target_properties(zlib PROPERTIES
      IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/external/lib/zlib.lib)
  else()
    set_target_properties(zlib PROPERTIES
      IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/external/lib/libz.a)
  endif()
  set(ZLIB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/external/include)
  set(ZLIB_LIBRARIES zlib)
  set(ZLIB_USE_EXTERNAL_PROJECT YES)
else()
  set(ZLIB_USE_EXTERNAL_PROJECT NO)
endif()
