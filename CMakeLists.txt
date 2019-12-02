cmake_minimum_required(VERSION 3.7)
add_definitions(-std=c++17)
project(AlignImage)
set(CXX_FLAGS "-Wall -lstdc++fs -lc++fs -lstdc++")

# Add thirdparty JSON library dependency
add_subdirectory(thirdparty/jsoncpp)
get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
option(JSONCPP_WITH_TESTS "" OFF) # Disable testing of the jsoncpp library
include_directories(${JSON_INC_PATH})

#find_package(OpenCV REQUIRED PATHS /usr/local/opt/OpenCV NO_DEFAULT_PATH) # manual specification of the OpenCVConfig.cmake path is necessary since otherwise, cmake assigns /opt/local a higher priority and ocv2.4 will be used
#set(CMAKE_EXE_LINKER_FLAGS, "$(pkg-config --cflags --libs /usr/local/Cellar/opencv/4.1.1_2/lib/pkgconfig/opencv4.pc )")
#set(ENV{PKG_CONFIG_PATH}  "$ENV{PKG_CONFIG_PATH}:/usr/local/Cellar/opencv/4.1.1_2/lib/pkgconfig/")
#find_package(PkgConfig REQUIRED)
#pkg_search_module(PKG_OPENCV REQUIRED opencv4) 
#include_directories(${PKG_OPENCV_INCLUDE_DIRS})
#target_link_libraries (FINAL_BINARY ${PKG_OPENCV_LDFLAGS})
#set(OpenCV_INCLUDE_DIRS "/usr/local/Cellar/opencv/4.1.1_2/include/opencv4/opencv2/")

# Add openCV dependency
set(OpenCV_INCLUDE_DIRS "/usr/local/opt/opencv/include/opencv4/opencv2/")
set(OpenCV_LIBS "/usr/local/opt/opencv/lib/")
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
include_directories(${OpenCV_INCLUDE_DIRS})

file(GLOB SOURCES
    src/*.h
    src/*.cpp
)

set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")
#target_include_directories(AlignImage PRIVATE src/*)
add_executable(AlignImage ${SOURCES})
target_link_libraries(AlignImage ${OpenCV_LIBS} jsoncpp_lib)