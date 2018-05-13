cmake_minimum_required(VERSION 2.8.12)

SET(CMAKE_INSTALL_PREFIX /home/red/Qt1/5.10.0/gcc_64/lib/cmake/Qt5Widgets/)

project(runa)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)
include_directories(${Qt5Widgets_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

get_target_property(QtCore_location Qt5::Core LOCATION)

# Create a debug build
set(CMAKE_CXX_FLAGS "-Wall --std=c++14 -Ofast")

# Set all CPP files to be source files
file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} ${Qt5Widgets_LIBRARIES})
