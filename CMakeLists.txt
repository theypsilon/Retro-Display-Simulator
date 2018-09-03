project ( retro-drawer )

cmake_minimum_required(VERSION 3.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (WIN32)
	set(SDL2_DIR "C:\\Users\\Jose\\Downloads\\SDL2-devel-2.0.8-VC\\SDL2-2.0.8")
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

add_executable(retro-drawer "src/main.cpp")

if (WIN32)
	add_custom_command(TARGET retro-drawer POST_BUILD
     COMMAND ${CMAKE_COMMAND} -E copy_if_different
         "${SDL2_DIR}/lib/x64/SDL2.dll"
         $<TARGET_FILE_DIR:retro-drawer>)

	add_custom_command(
        TARGET retro-drawer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/resources
                ${CMAKE_CURRENT_BINARY_DIR}/resources
		DEPENDS ${CMAKE_CURRENT_BINARY_DIR})
endif()

target_link_libraries(retro-drawer ${LIBS})
