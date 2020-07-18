# Copyright (c) 2020 Koki Fukuda
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

enable_testing()
find_package(Boost COMPONENTS unit_test_framework)

add_custom_target(test_executable)

function(add_boost_test target_name test_name)
  if(NOT Boost_FOUND)
    return()
  endif()

  add_executable(${target_name} EXCLUDE_FROM_ALL ${ARGN})
  target_include_directories(${target_name} PRIVATE SYSTEM ${Boost_INCLUDE_DIRS})
  target_compile_definitions(${target_name} PRIVATE "BOOST_TEST_DYN_LINK=1")
  target_compile_definitions(${target_name} PRIVATE "BOOST_TEST_MAIN")
  target_link_libraries(${target_name} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

  add_dependencies(test_executable ${target_name})

  add_test(NAME ${test_name} COMMAND ${target_name})
endfunction()
