# the minimum version of CMake required
cmake_minimum_required(VERSION 3.6.2)

set (CMAKE_CXX_COMPILER "clang++" )
set (CMAKE_C_COMPILER "clang" )
set (CMAKE_CXX_STANDARD 14)

project( vimol VERSION 0.1 LANGUAGES CXX C )

message( "${CMAKE_SOURCE_DIR} abc ")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/src/cmake")

find_package(Cairo REQUIRED cairo)
find_package(SDL2 REQUIRED)

add_subdirectory(src)

# Properties->General->Output Directory
set_target_properties( ${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR} )
