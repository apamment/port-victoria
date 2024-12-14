cmake_minimum_required(VERSION 3.6)

project(portvic)

set(CMAKE_CXX_STANDARD 11)

find_package(SQLite3 REQUIRED)

add_custom_target(
   odoors
   COMMAND make
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/odoors
)

add_executable(portvic main.c marketplace.c ports.c shipyard.c tavern.c)
include_directories(${CMAKE_SOURCE_DIR}/odoors ${SQLite3_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(portvic ${CMAKE_SOURCE_DIR}/odoors/libs-${CMAKE_SYSTEM_NAME}/libODoors.a ${SQLite3_LIBRARIES})
add_dependencies(portvic odoors)