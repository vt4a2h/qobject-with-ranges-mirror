cmake_minimum_required(VERSION 3.0.0)

project(qobjects_ranges VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/../range-v3/include
                           ${CMAKE_SOURCE_DIR}/../src)

find_package(Qt5Core)

add_executable(${PROJECT_NAME} "main.cpp" "FooBar.h" "../src/qrange.h")
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
