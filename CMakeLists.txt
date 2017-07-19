
cmake_minimum_required(VERSION 2.6.3)

message( STATUS "cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(cdobs C CXX)

#set(CMAKE_CXX_FLAGS "")

find_package(SQLite3 REQUIRED)

add_subdirectory(src)

