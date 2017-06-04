cmake_minimum_required(VERSION 3.7)
project(pixy_roimux)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
find_package(ROOT REQUIRED Spectrum)
add_definitions(${ROOT_CXX_FLAGS})
include_directories(${ROOT_INCLUDE_DIR})

set(SOURCE_FILES main.cpp ChargeData.cpp ChargeData.h RunParams.cpp RunParams.h Event.cpp Event.h ChargeHits.cpp ChargeHits.h)
add_executable(pixy_roimux ${SOURCE_FILES})

target_link_libraries(pixy_roimux ${ROOT_LIBRARIES})

#install(TARGETS pixy_roimux DESTINATION bin)