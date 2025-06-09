cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(lights_monitor)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Enable debug symbols by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

# Export compile commands for completion engines (optional)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(libs)

# Add executable
add_executable(${PROJECT_NAME} src/main.cpp)

# Using INSTALL_RPATH_USE_LINK_PATH
set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

# Set executable install location
install(TARGETS ${PROJECT_NAME} DESTINATION build)
