project ( retro-voxel-display )

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (WIN32)
	set(SDL2_DIR $ENV{SDLDIR})
endif()

find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)


set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl")

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)
include_directories("${CMAKE_SOURCE_DIR}/includes/" "${SDL2_INCLUDE_DIRS}")

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

set(LIBS ${LIBS} "${SDL2_LIBRARIES}")
if (WIN32)
	add_executable(retro-voxel-display "src/main.cpp")
	add_custom_command(TARGET retro-voxel-display POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "${SDL2_DIR}/lib/x64/SDL2.dll"
         $<TARGET_FILE_DIR:retro-voxel-display>)
else()
	add_executable(retro-voxel-display "src/main.cpp")
endif()

target_link_libraries(retro-voxel-display ${LIBS})
