cmake_minimum_required(VERSION 3.10)
project(imagesampler)

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

# Try to find OpenCV
if(APPLE)
set(OpenCV_DIR "/Users/venkatapydialli/Projects/Edgetpu/install/lib/cmake/opencv4")
else()
set(OpenCV_DIR "/usr")
endif()

find_package(OpenCV REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/libs/)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/datasketches)

add_library(imagesampler SHARED
            src/helpers/saver.cpp
            src/helpers/iniparser.cpp
            src/helpers/imghelpers.cpp
	    src/helpers/generic.cpp
            src/sampling/imagesampler.cpp
			)

add_library(imageprofiler SHARED
			src/helpers/saver.cpp
			src/helpers/iniparser.cpp
                        src/helpers/imghelpers.cpp
			src/helpers/generic.cpp
			src/profiles/imageprofile.cpp
			)

add_library(modelprofiler SHARED 
            src/helpers/saver.cpp
            src/helpers/iniparser.cpp
	    src/helpers/generic.cpp
            src/profiles/modelprofile.cpp
            )	    

if(APPLE)
message(STATUS "skipping TEST for MAC")
else()
add_executable(Tar_GZ_test
            src/helpers/tar_gz_creator.cpp
            src/helpers/tests/tar_gz_creator_test.cpp
            )


add_executable(Http_uploader_test
	    src/helpers/http_uploader.cpp
            src/helpers/tests/http_uploader_test.cpp	    
	    )   


add_executable(ImageProcessingTest
                src/helpers/imghelpers.cpp
                src/helpers/tests/imagehelpers_test.cpp
              )

add_executable(IniParserTest
                src/helpers/iniparser.cpp
                src/helpers/tests/iniparser_test.cpp
              )

add_executable(SaverTest
                src/helpers/saver.cpp
                src/helpers/tests/saver_test.cpp
              )

add_executable(image_profiler_test
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
                src/helpers/imghelpers.cpp
		src/helpers/generic.cpp
                src/profiles/imageprofile.cpp
                src/profiles/tests/imageprofile_test.cpp
              )
add_executable(model_profiler_test
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
		src/helpers/generic.cpp
                src/profiles/modelprofile.cpp
                src/profiles/tests/modelprofile_test.cpp
	      )

add_executable(image_sampler_test
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
                src/helpers/imghelpers.cpp
		src/helpers/generic.cpp
                src/sampling/imagesampler.cpp
                src/sampling/tests/imagesampler_test.cpp
              )

target_compile_definitions(ImageProcessingTest PRIVATE TEST)

target_compile_definitions(IniParserTest PRIVATE TEST)
target_compile_definitions(SaverTest PRIVATE TEST)
target_compile_definitions(image_profiler_test PRIVATE TEST)
target_compile_definitions(image_sampler_test PRIVATE TEST)
target_compile_definitions(model_profiler_test PRIVATE TEST)
target_compile_definitions(Http_uploader_test PRIVATE TEST)
target_compile_definitions(Tar_GZ_test PRIVATE TEST)

target_link_libraries(ImageProcessingTest gtest gtest_main ${OpenCV_LIBS} pthread curl)
target_link_libraries(IniParserTest gtest gtest_main ${OpenCV_LIBS} pthread curl)
target_link_libraries(SaverTest gtest gtest_main ${OpenCV_LIBS} pthread)
target_link_libraries(image_profiler_test gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
target_link_libraries(image_sampler_test gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
target_link_libraries(model_profiler_test gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
target_link_libraries(Http_uploader_test gtest gtest_main ${OpenCV_LIBS} ${CURL_LIBRARIES} curl pthread)
target_link_libraries(Tar_GZ_test gtest gtest_main tar z boost_filesystem boost_system pthread)

enable_testing()
#Test
add_test(NAME ImageProcessingTest COMMAND ImageProcessingTest)
add_test(NAME IniParserTest COMMAND IniParserTest)
add_test(NAME SaverTest COMMAND SaverTest)
add_test(NAME image_profiler_test COMMAND image_profiler_test)
add_test(NAME image_sampler_test COMMAND image_sampler_test)
add_test(NAME model_profiler_test COMMAND model_profiler_test)
#add_test(NAME  COMMAND )
endif()

target_link_libraries(imagesampler ${OpenCV_LIBS} pthread curl)
target_link_libraries(imageprofiler ${OpenCV_LIBS} pthread curl)
target_link_libraries(modelprofiler ${OpenCV_LIBS} pthread curl)

# Install the library
install(TARGETS ${CMAKE_PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


add_subdirectory(examples/TFLite/Classification_Binary/ EXCLUDE_FROM_ALL)
