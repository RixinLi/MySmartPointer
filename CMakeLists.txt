cmake_minimum_required(VERSION 2.8)
project(SmartPointer)

set(CMAKE_VERBOSE_MAKEFILE ON)

#-Werror
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")


add_executable(test_SmartPointer test_SmartPointer.cc)
add_dependencies(test_SmartPointer SmartPointer)
target_link_libraries(test_SmartPointer "${LIBS}")