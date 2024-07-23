cmake_minimum_required(VERSION 3.10)
project(TFLiteCheck)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Ensure builds on Windows export all symbols
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g3 -DDEBUG=1")
set(CMAKE_CXX_FLAGS_RELEASE "-DDEBUG=0")

#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_MACOSX_RPATH ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Code generation options, to ensure shaerd libraries work and are portable
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# enable compiler warnings globally
# derived from https://foonathan.net/blog/2018/10/17/cmake-warnings.html
# and https://arne-mertz.de/2018/07/cmake-properties-options/
if (MSVC)
  add_compile_options(/W4)
  set(CMAKE_DEBUG_POSTFIX "d")
else()
  add_compile_options(-Wall -pedantic -W -Wextra -lstdc++fs)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  add_compile_options(-Wimplicit-fallthrough=3)
endif()

# Set the AWS service components used by this project.
#set(SERVICE_COMPONENTS s3)

# Find the AWS SDK for C++ package.
#find_package(AWSSDK REQUIRED COMPONENTS ${SERVICE_COMPONENTS})

# Find libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIRS})

find_package(ZLIB REQUIRED)
find_library(TAR_LIB tar REQUIRED)
	
if(NOT TAR_LIB)
    message(FATAL_ERROR "libtar not found")
endif()
# Try to find OpenCV
if(APPLE)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/install/lib/cmake/opencv4")
else()
set(OpenCV_DIR "/usr")
endif()

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
message(STATUS "OpenCV_DIR: ${OpenCV_DIR}")

# Check if CMAKE_SOURCE_DIR contains "example"
string(FIND "${CMAKE_SOURCE_DIR}" "/example" EXAMPLE_FOUND)
if(EXAMPLE_FOUND GREATER -1)
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../../)
else()
    set(SOURCE_DIR ${CMAKE_SOURCE_DIR})
endif()

INCLUDE_DIRECTORIES(${SOURCE_DIR}/include/datasketches)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/build)
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/local/include/lib/)
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/libs/)

if(APPLE)
# TensorFlow Lite Integration
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tflite-dist/include/)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tflite-dist/libs/)
else()
message(STATUS "NOT apple: ${OpenCV_DIR}")
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tensorflow_src/)
INCLUDE_DIRECTORIES(${SOURCE_DIR}/../tflite_v2/flatbuffers/include/)
endif()
add_executable(${PROJECT_NAME} main.cpp)

    target_link_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR}/build/)
if(APPLE)
    target_link_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR}/../tflite-dist/libs/mac/)
else()
    target_link_directories(${PROJECT_NAME} PRIVATE ${SOURCE_DIR}/../tflite_v2/)
endif()
    target_link_libraries(TFLiteCheck tensorflowlite ${OpenCV_LIBS} imagesampler imageprofiler modelprofiler tar curl pthread)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/../tflite-dist/libs/windows_x64/tensorflowlite.dll ${CMAKE_BINARY_DIR}/Release/
    )

    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${OpenCV_DIR}/x64/vc15/bin/opencv_world452.dll ${CMAKE_BINARY_DIR}/Release/
    )
endif()
