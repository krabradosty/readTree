cmake_minimum_required(VERSION 3.5)
project(readTree)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net)

include(${ROOT_USE_FILE})
include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

add_executable(readTree main.cpp)
target_link_libraries(readTree ${ROOT_LIBRARIES})














