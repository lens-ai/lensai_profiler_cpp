cmake_minimum_required(VERSION 3.10)
project(lensai)

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

find_package (Eigen3 REQUIRED NO_MODULE)
find_package(OpenCV REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(/usr/local/include/)
INCLUDE_DIRECTORIES(/usr/local/libs/)

# Include Directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include/datasketches)
include_directories(${ZLIB_INCLUDE_DIRS})

set(TARGET_LIBS imagesampler imageprofiler modelprofiler customprofiler lensaipublisher trackingprofiler)

add_library(imagesampler SHARED
            src/helpers/generic.cpp
            src/helpers/saver.cpp
            src/helpers/iniparser.cpp
            src/helpers/imghelpers.cpp
            src/sampling/imagesampler.cpp
			)

add_library(imageprofiler SHARED
            src/helpers/generic.cpp
            src/helpers/saver.cpp
            src/helpers/iniparser.cpp
            src/helpers/imghelpers.cpp
            src/profiles/imageprofile.cpp
			)

add_library(modelprofiler SHARED 
            src/helpers/generic.cpp
            src/helpers/saver.cpp
            src/helpers/iniparser.cpp
            src/profiles/modelprofile.cpp
            )

add_library(customprofiler SHARED
	    src/helpers/generic.cpp
	    src/helpers/saver.cpp
	    src/helpers/iniparser.cpp
	    src/profiles/customprofile.cpp
           )

add_library(trackingprofiler SHARED
            src/helpers/generic.cpp
	    src/helpers/saver.cpp
	    src/helpers/iniparser.cpp
	    src/profiles/trackingprofile.cpp
	    src/helpers/trackingmetrics.cpp
)	   

add_library(lensaipublisher SHARED
            src/helpers/iniparser.cpp
            src/helpers/generic.cpp
            src/helpers/tar_gz_creator.cpp
            src/helpers/http_uploader.cpp
            )
	    
if(APPLE)
message(STATUS "skipping TEST for MAC")
else()
add_executable(Tar_GZ_test
            src/helpers/tar_gz_creator.cpp
            src/helpers/tests/tar_gz_creator_test.cpp
            )


#add_executable(Http_uploader_test
#	    src/helpers/http_uploader.cpp
#            src/helpers/tests/httpuploader_test.cpp	    
#	    )   


add_executable(ImageProcessingTest
                src/helpers/imghelpers.cpp
                src/helpers/tests/imagehelpers_test.cpp
              )

add_executable(IniParserTest
                src/helpers/iniparser.cpp
                src/helpers/tests/iniparser_test.cpp
              )

add_executable(SaverTest
                src/helpers/generic.cpp
                src/helpers/saver.cpp
                src/helpers/tests/saver_test.cpp
              )

add_executable(ImageProfilerTest
                src/helpers/generic.cpp
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
                src/helpers/imghelpers.cpp
                src/profiles/imageprofile.cpp
                src/profiles/tests/imageprofile_test.cpp
              )

add_executable(ModelProfilerTest
                src/helpers/generic.cpp
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
                src/profiles/modelprofile.cpp
                src/profiles/tests/modelprofile_test.cpp
              )

add_executable(ImageSamplerTest
                src/helpers/generic.cpp
                src/helpers/saver.cpp
                src/helpers/iniparser.cpp
                src/helpers/imghelpers.cpp
                src/sampling/imagesampler.cpp
                src/sampling/tests/imagesampler_test.cpp
              )

add_executable(TrackingMetricsTest
	        src/helpers/trackingmetrics.cpp
		src/helpers/tests/trackingmetrics_test.cpp
	      )
	       

target_compile_definitions(ImageProcessingTest PRIVATE TEST)
target_compile_definitions(IniParserTest PRIVATE TEST)
target_compile_definitions(SaverTest PRIVATE TEST)
target_compile_definitions(ImageProfilerTest PRIVATE TEST)
target_compile_definitions(ImageSamplerTest PRIVATE TEST)
target_compile_definitions(ModelProfilerTest PRIVATE TEST)
#target_compile_definitions(Http_uploader_test PRIVATE TEST)
target_compile_definitions(Tar_GZ_test PRIVATE TEST)
target_compile_definitions(TrackingMetricsTest PRIVATE TEST)

target_link_libraries(ImageProcessingTest gtest gtest_main ${OpenCV_LIBS} pthread curl)
target_link_libraries(IniParserTest gtest gtest_main ${OpenCV_LIBS} pthread curl)
target_link_libraries(SaverTest gtest gtest_main ${OpenCV_LIBS} pthread)
target_link_libraries(ImageProfilerTest gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
target_link_libraries(ImageSamplerTest gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
target_link_libraries(ModelProfilerTest gtest gtest_main ${OpenCV_LIBS} ${AWSSDK_LINK_LIBRARIES} curl pthread)
#target_link_libraries(Http_uploader_test gtest gtest_main ${OpenCV_LIBS} ${CURL_LIBRARIES} curl pthread)
target_link_libraries(Tar_GZ_test gtest gtest_main tar z boost_filesystem boost_system pthread)
target_link_libraries(TrackingMetricsTest gtest gtest_main ${OpenCV_LIBS} pthread curl Eigen3::Eigen) 

enable_testing()
#Test
add_test(NAME ImageProcessingTest COMMAND ImageProcessingTest)
add_test(NAME IniParserTest COMMAND IniParserTest)
add_test(NAME SaverTest COMMAND SaverTest)
add_test(NAME ImageProfilerTest COMMAND ImageProfilerTest)
add_test(NAME ImageSamplerTest COMMAND ImageSamplerTest)
add_test(NAME ModelSamplerTest COMMAND ModelSamplerTest)
add_test(NAME TrackingMetricsTest COMMAND TrackingMetricsTest)
#add_test(NAME  COMMAND )
endif()

target_link_libraries(imagesampler ${OpenCV_LIBS} pthread curl)
target_link_libraries(imageprofiler ${OpenCV_LIBS} pthread curl)
target_link_libraries(modelprofiler ${OpenCV_LIBS} pthread curl)
target_link_libraries(customprofiler ${OpenCV_LIBS} pthread curl)
target_link_libraries(trackingprofiler ${OpenCV_LIBS} pthread curl Eigen3::Eigen)
target_link_libraries(lensaipublisher ${OpenCV_LIBS} pthread curl ${ZLIB_LIBRARIES} ${TAR_LIB})

# Install the library
install(TARGETS ${TARGET_LIBS}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include)


add_subdirectory(examples/TFLite/Classification_Binary/ EXCLUDE_FROM_ALL)
