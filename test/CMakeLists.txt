cmake_minimum_required(VERSION 3.0.0)

project(tst_qobject_ranges VERSION 0.1 LANGUAGES CXX)

include(CheckCXXCompilerFlag)

set(QRANGE_IMPL_INCLUDE "../src/qrangefallback_p.h")

check_cxx_compiler_flag("-fconcepts" CXX_CONCEPTS_FLAG)
if(CXX_CONCEPTS_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
endif()

check_cxx_compiler_flag("-fcoroutines-ts" CXX_COROUTINES_FLAG)
if(CXX_COROUTINES_FLAG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -stdlib=libc++")
  set(QRANGE_IMPL_INCLUDE "../src/qrangecoro_p.h")

  add_definitions(-DCOROUTINES_SUPPORT)

  include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/../cppcoro/include)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/../range-v3/include
                           ${CMAKE_SOURCE_DIR}/../src)

find_package(Qt5Core)

set(INCLUDES "FooBar.h" "../src/qrange.h" ${QRANGE_IMPL_INCLUDE})
if(CMAKE_BUILD_TYPE EQUAL "RELEASE")
    set(INCLUDES "${INCLUDES} benchmarks.h")
endif(CMAKE_BUILD_TYPE EQUAL "RELEASE")

add_executable(${PROJECT_NAME} "main.cpp" ${INCLUDES})

if(APPLE AND ${CMAKE_VERSION} VERSION_LESS "3.15.0" AND ${CMAKE_CXX_COMPILER_ID} MATCHES "AppleClang")
  set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std=c++2a")
  set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std=gnu++2a")
endif()

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} Qt5::Core)
