cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
set(PROJECT_NAME test_c_str)
set(LIB_NAME my_c_string)

project(${PROJECT_NAME} C CXX)

#! Options
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)
option(ENABLE_PVS_STUDIO "Check using command-line PVS-Studio." ON)
#! ENABLE_SANITIZERS is the option for the test builds!
#  Definitely enable it while developing.
#  Disable it for the production builds and before submitting for grading!
option(ENABLE_SANITIZERS "Use sanitizers to detect errors." OFF)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

#####################################

#! Make your c_strings library
add_library(
  ${LIB_NAME} SHARED 
  ${CMAKE_SOURCE_DIR}/c_str_lib/c_string.c
  ${CMAKE_SOURCE_DIR}/c_str_lib/c_string.h)

target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/c_str_lib)

#####################################

#! Project source compilation
add_executable(${PROJECT_NAME} src/main.c)

target_link_libraries(${PROJECT_NAME} ${LIB_NAME})

#####################################

#! Specify output directories
#! For different tests etc it should be `bin`. For shared library it should be `lib`
set_target_properties(${LIB_NAME} ${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(${LIB_NAME}
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set (ALL_TARGETS ${PROJECT_NAME} ${LIB_NAME})

# Include fixed CMake configuration
include(cmake/config.cmake)
 
