cmake_minimum_required(VERSION 3.22)
project(TopoMapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE "TopoMapper" CACHE STRING "Executable name")

# get source files from src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# get header files from include directory
file(GLOB_RECURSE HEADERS "include/*.h")

# Specify the path for the configuration file
set(CONFIG_FILE "${CMAKE_BINARY_DIR}/config.h")

# Use configure_file to create the header with the ROOT_PATH definition
configure_file(
    ${CMAKE_SOURCE_DIR}/config.h.in
    ${CONFIG_FILE}
)

# Include the generated configuration file in your project
include_directories(${CMAKE_BINARY_DIR})



#find gaussian renderer
add_subdirectory(extlibs/splatapult)



include_directories(
    extlibs/splatapult/src
)

# find Eigen3 package
find_package(Eigen3 REQUIRED)

find_package(glfw3 REQUIRED)

# include glfw3 headers
include_directories(${GLFW_INCLUDE_DIRS})

#find OpenGL packages
find_package(OpenGL REQUIRED)

# include OpenGL headers
include_directories(${OPENGL_INCLUDE_DIRS})

# find GLEW package
find_package(GLEW REQUIRED)

# include GLEW headers
include_directories(${GLEW_INCLUDE_DIRS})

# include Eigen3 headers
include_directories(${EIGEN3_INCLUDE_DIR})

# add executable
add_executable(${EXECUTABLE} ${SOURCES} ${HEADERS})

# add include directories
target_include_directories(${EXECUTABLE} PRIVATE include)

target_link_libraries(${EXECUTABLE} Eigen3::Eigen glfw GLEW::GLEW ${OPENGL_LIBRARIES} splatapult)


