project ( retro-voxel-display )

cmake_minimum_required(VERSION 3.10)
message(CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE})

set(PROJECT_VERSION 0.1.0)
set(PROJECT_OFFICIAL_NAME "Retro Voxel Display")
set(PROJECT_BINARY_NAME rvd)


add_definitions( -DPROJECT_VERSION="${PROJECT_VERSION}" -DPROJECT_OFFICIAL_NAME="${PROJECT_OFFICIAL_NAME}" -DPROJECT_BINARY_NAME="${PROJECT_BINARY_NAME}")
configure_file(configuration/version.txt.in ${CMAKE_SOURCE_DIR}/configuration/version.var)
configure_file(configuration/binary_name.txt.in ${CMAKE_SOURCE_DIR}/configuration/binary_name.var)
configure_file(configuration/project_name.txt.in ${CMAKE_SOURCE_DIR}/configuration/project_name.var)

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
set( GLFW_USE_HYBRID_HPG ON CACHE INTERNAL "GLFW use hybrid HPG.")

add_subdirectory( glfw )

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}") # -ldl

configure_file(configuration/cmake_source_dir.h.in configuration/cmake_source_dir.h)
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
	add_executable(${PROJECT_BINARY_NAME} WIN32 ${SRCS})
	add_custom_command(TARGET ${PROJECT_BINARY_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_BINARY_NAME}.exe
			${CMAKE_SOURCE_DIR})
else()
	add_executable(${PROJECT_BINARY_NAME} ${SRCS})
endif()

target_link_libraries(${PROJECT_BINARY_NAME} ${LIBS})

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	target_compile_definitions(${PROJECT_BINARY_NAME} PRIVATE DEBUG)
endif()

if  (MSVC)
else()
	target_compile_options(${PROJECT_BINARY_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()