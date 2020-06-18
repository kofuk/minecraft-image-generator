find_package(ZLIB)
if(NOT ZLIB_FOUND)
  include(ExternalProject)
  ExternalProject_Add(zlib_project
    URL https://zlib.net/zlib-1.2.11.tar.xz
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/external -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE})
  set(ZLIB_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/external/include)
  set(ZLIB_LIBRARIES zlib)
  if(MSVC)
    if(CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
      set(ZLIB_LIBRARY ${PROJECT_SOURCE_DIR}/external/lib/zlibstaticd.lib)
    else()
      set(ZLIB_LIBRARY ${PROJECT_SOURCE_DIR}/external/lib/zlibstatic.lib)
    endif()
  else()
    set(ZLIB_LIBRARY ${PROJECT_SOURCE_DIR}/external/lib/libz.a)
  endif()
  add_library(zlib IMPORTED STATIC)
  if(MSVC)
    set_target_properties(zlib PROPERTIES
      IMPORTED_LOCATION ${ZLIB_LIBRARY})
  else()
    set_target_properties(zlib PROPERTIES
      IMPORTED_LOCATION ${ZLIB_LIBRARY})
  endif()
  set(ZLIB_USE_EXTERNAL_PROJECT YES)
else()
  set(ZLIB_USE_EXTERNAL_PROJECT NO)
endif()
