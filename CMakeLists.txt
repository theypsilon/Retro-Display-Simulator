project ( retro-voxel-display )

cmake_minimum_required(VERSION 3.10)

message(CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)

include_directories("glfw/include")

set( ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Build basic example applications" )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_TESTS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_BUILD_INSTALL OFF CACHE BOOL  "GLFW lib only" )

add_subdirectory( glfw )

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -ldl -lstdc++fs")

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)
include_directories("${CMAKE_SOURCE_DIR}/includes/")

# first create relevant static libraries requried for other projects
add_library(STB_IMAGE "src/stb_image.cpp")
set(LIBS ${LIBS} STB_IMAGE)

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)
set(LIBS ${LIBS} glfw ${GLFW_LIBRARIES})

set(SRCS ${SRCS} "src/main.cpp" "src/resources.cpp")
if (WIN32 AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	set(SRCS ${SRCS} "src/win_main.cpp")
	add_executable(retro-voxel-display WIN32 ${SRCS})
	add_custom_command(TARGET retro-voxel-display POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/retro-voxel-display.exe
			${CMAKE_SOURCE_DIR})
else()
	add_executable(retro-voxel-display ${SRCS})
endif()

target_link_libraries(retro-voxel-display ${LIBS})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	target_compile_definitions(retro-voxel-display PRIVATE DEBUG)
endif()
