cmake_minimum_required(VERSION 2.8)

# Project dirt
project(dirt)

# Set major and minor version, used in config.h.in
set(PROJECT_VERSION "0.1")

# Use doxygen cmake module, for make doc target
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(DOXYFILE_HTML_DIR "./")
include(UseDoxygen)

# Add dirt sources
set(dirt_SOURCES dirt.cpp)

# Add a configured file
configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
			   "${PROJECT_BINARY_DIR}/config.h")
# Allow searching in binary-tree (to find configured sources)
include_directories("${PROJECT_BINARY_DIR}")

# Add xmlsp as search directory
include_directories("${PROJECT_SOURCE_DIR}/XMLSP")
include_directories("${PROJECT_SOURCE_DIR}")
# Add subdirectory projects
add_subdirectory(XMLSP)
add_subdirectory(DRT)
add_subdirectory(Tests)

# Add executable target
add_executable(dirt ${dirt_SOURCES})

# Link against xmlsp
target_link_libraries(dirt drt xmlsp)
