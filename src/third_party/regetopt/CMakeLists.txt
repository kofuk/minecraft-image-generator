# SPDX-License-Identifier: MIT

add_library(regetopt STATIC regetopt.c)
target_include_directories(regetopt SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
