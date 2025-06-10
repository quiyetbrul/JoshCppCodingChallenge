set(LIGHT_SOURCES
    light_monitor.cpp
)

add_library(light ${LIGHT_SOURCES})

target_include_directories(light PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
