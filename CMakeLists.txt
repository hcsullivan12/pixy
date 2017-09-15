cmake_minimum_required(VERSION 3.5)
project(pixy)

set(CMAKE_CXX_STANDARD 11)

list(APPEND CMAKE_MODULE_PATH $ENV{ROOTSYS}/etc/cmake)
find_package(ROOT COMPONENTS Geom REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS})

set(Eigen3_DIR $ENV{Eigen3_CMAKEDIR})
find_package(Eigen3 REQUIRED CONFIG)
include_directories(${EIGEN3_INCLUDE_DIRS})

set(RapidJSON_DIR $ENV{RapidJSON_CMAKEDIR})
find_package(RapidJSON REQUIRED CONFIG)
include_directories(${RapidJSON_INCLUDE_DIRS})

find_library(GENFIT_LIBRARIES NAMES libgenfit2.so PATHS $ENV{GENFIT}/lib)
include_directories($ENV{GENFIT}/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE headers ${PROJECT_SOURCE_DIR}/include/*.h)
add_executable(pixy main.cpp ${sources} ${headers})

target_link_libraries(pixy ${ROOT_LIBRARIES} ${GENFIT_LIBRARIES})


