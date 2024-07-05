cmake_minimum_required(VERSION 3.22)
project(TopoMapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE "TopoMapper" CACHE STRING "Executable name")

# get source files from src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# get header files from include directory
file(GLOB_RECURSE HEADERS "include/*.h")

# find Eigen3 package
find_package(Eigen3 REQUIRED)

# include Eigen3 headers
include_directories(${EIGEN3_INCLUDE_DIR})

# add executable
add_executable(${EXECUTABLE} ${SOURCES} ${HEADERS})

# add include directories
target_include_directories(${EXECUTABLE} PRIVATE include)

target_link_libraries(${EXECUTABLE} Eigen3::Eigen)

