cmake_minimum_required(VERSION 3.10)
project(gravitar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

#######################
# Compile definitions
###

# Project directory
add_compile_definitions(GRAVITAR_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}")

################
# Dependencies
###

# Include deps folder to include path
include_directories(deps)

# Add directory containing FindSFML.cmake to module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/;${CMAKE_MODULE_PATH}")

# Find any version 2.X of SFML
# See the FindSFML.CMake file for additional details and instructions
find_package(SFML 2.4 COMPONENTS system window graphics audio REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

#################
# Executable
###

include_directories(src)

file(GLOB HEADERS src/*.hpp src/**/*.hpp)
file(GLOB SOURCES src/*.cpp src/**/*.cpp)

add_executable(gravitar ${HEADERS} ${SOURCES})
target_link_libraries(gravitar PRIVATE ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
