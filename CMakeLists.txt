cmake_minimum_required(VERSION 3.10)
project(nongravitar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-error=deprecated-declarations")

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

add_executable(nongravitar ${HEADERS} ${SOURCES})
target_link_libraries(nongravitar PRIVATE ${SFML_LIBRARIES} ${SFML_DEPENDENCIES})
target_compile_definitions(nongravitar PRIVATE NONGRAVITAR_DIRECTORY="${CMAKE_CURRENT_LIST_DIR}")
target_compile_definitions(nongravitar PRIVATE NONGRAVITAR_FONTS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/fonts")
target_compile_definitions(nongravitar PRIVATE NONGRAVITAR_SOUNDS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/sounds")
target_compile_definitions(nongravitar PRIVATE NONGRAVITAR_SOUNDTRACKS_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/soundtracks")
target_compile_definitions(nongravitar PRIVATE NONGRAVITAR_TEXTURES_PATH="${CMAKE_CURRENT_LIST_DIR}/assets/textures")
