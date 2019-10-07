cmake_minimum_required(VERSION 3.10)
project(gravitar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")

#######################
# Compile definitions
###

add_compile_definitions(GRAVITAR_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}")
add_compile_definitions(GRAVITAR_FONTS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/fonts")
add_compile_definitions(GRAVITAR_SOUNDS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/sounds")
add_compile_definitions(GRAVITAR_SOUNDTRACKS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/soundtracks")
add_compile_definitions(GRAVITAR_TEXTURES_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/textures")

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

file(GLOB HEADERS ${CMAKE_CURRENT_LIST_DIR}/src/*.hpp ${CMAKE_CURRENT_LIST_DIR}/src/**/*.hpp)
file(GLOB SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp ${CMAKE_CURRENT_LIST_DIR}/src/**/*.cpp)

add_executable(gravitar ${HEADERS} ${SOURCES})
target_link_libraries(gravitar PRIVATE ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
