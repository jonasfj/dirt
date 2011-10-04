cmake_minimum_required(VERSION 2.8)

#TODO: Check that NDEBUG is set in release mode

# Project dirt
project(dirt)

# Set major and minor version, used in config.h.in
set(dirt_VERSION_MAJOR 1)
set(dirt_VERSION_MINOR 0)

# Add dirt sources
set(dirt_SOURCES 
		DotDRTBuilder.cpp
		Main.cpp
		MatrixDRTBuilder.cpp
		MatrixTask.cpp
		RandomDRTGenerator.cpp
		TeeDRTBuilder.cpp
		ValidatingDRTBuilder.cpp
		WangsDBFAlgorithm.cpp
		WangsUtilizationAlgorithm.cpp
		XmlDRTBuilder.cpp
		XmlDRTParser.cpp
	)

# Add a configured file
configure_file("${PROJECT_SOURCE_DIR}/config.h.in"
			   "${PROJECT_BINARY_DIR}/config.h")
# Allow searching in binary-tree (to find configured sources)
include_directories("${PROJECT_BINARY_DIR}")

# Add xmlsp as search directory
include_directories("${PROJECT_SOURCE_DIR}/xmlsp")
# Add subdirectory project
add_subdirectory(xmlsp)

# Add executable target
add_executable(dirt ${dirt_SOURCES})

# Link against xmlsp
target_link_libraries(dirt xmlsp)
