
cmake_minimum_required(VERSION 3.0.2)

message( STATUS "cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 11)

project(cdobs C CXX)

find_package(SQLITE3 REQUIRED)

add_subdirectory(src)

