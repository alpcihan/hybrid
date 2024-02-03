cmake_minimum_required(VERSION 3.13)
project(hybrid VERSION 1.0 LANGUAGES C CXX DESCRIPTION "hybrid")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
endif(CMAKE_COMPILER_IS_GNUCXX)
if(WIN32)
    add_definitions(-DNOMINMAX)
endif(WIN32)

# externals
find_package(Threads REQUIRED)
add_subdirectory(external/TGA)

# shaders
add_subdirectory(shaders)

# lib
configure_file(./include/hybrid/config/path.hpp.in include/hybrid/config/path.hpp)

set(HYBRID_LIB_NAME ${CMAKE_PROJECT_NAME}_lib)

file(GLOB_RECURSE HYBRID_SOURCE_FILES "./src/*.cpp")
add_library(${HYBRID_LIB_NAME} ${HYBRID_SOURCE_FILES})

target_include_directories(${HYBRID_LIB_NAME} 
    PUBLIC 
        ./include
        ${CMAKE_CURRENT_BINARY_DIR}/include
)
target_link_libraries(${HYBRID_LIB_NAME} PUBLIC tga_vulkan tga_utils ${CMAKE_THREAD_LIBS_INIT})
add_dependencies(${HYBRID_LIB_NAME} hybrid_shaders)

if(WIN32)
    set_property(TARGET ${HYBRID_LIB_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif(WIN32)

# examples
set(HYBRID_EXAMPLES_ENABLED ON)
if(HYBRID_EXAMPLES_ENABLED)
    add_subdirectory(examples)
endif()
