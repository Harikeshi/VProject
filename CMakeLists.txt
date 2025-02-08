cmake_minimum_required(VERSION 3.16)
#-------------------------------------------------------------------------------
project(VProject VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})

find_package(Qt5 COMPONENTS Widgets REQUIRED)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")


add_subdirectory(tests/)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/tests")
