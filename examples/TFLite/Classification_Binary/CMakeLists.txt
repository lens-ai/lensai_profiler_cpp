cmake_minimum_required(VERSION 3.17)
project(TFLiteCheck)

set(CMAKE_CXX_STANDARD 14)
if(APPLE)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/install/lib/cmake/opencv4")
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
    target_link_libraries(TFLiteCheck tensorflowlite ${OpenCV_LIBS} imagesampler imageprofiler modelprofiler)

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
