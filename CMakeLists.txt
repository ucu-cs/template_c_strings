cmake_minimum_required(VERSION 3.9)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
set(PROJECT_NAME test_c_str)
set(LIB_NAME my_c_string)

if (WIN32) # Fix for "fatal error U1073: don't know how to make library" on MSVC
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif ()

project(${PROJECT_NAME} C CXX)
#! Some systems, especially -- MacOS could require the following or similar lines.
# set(CMAKE_C_STANDARD 11)
# set(CMAKE_CXX_STANDARD 14)

#! Options
set(WARNINGS_AS_ERRORS ON)    # "Treat compiler warnings as errors."
set(ENABLE_PVS_STUDIO ON)    # "Check using command-line PVS-Studio."
#! ENABLE_SANITIZERS is the option for the test builds!
#       Definitely enable it while developing.
#       Disable it for the production builds and before submitting for grading!
set(ENABLE_SANITIZERS OFF)    # "Use sanitizers to detect errors."

# !Warnings as errors should be imported here.
# !Do not delete this line! switch it 'OFF' in case you don't need it.
include(cmake/defaults/CompilerWarnings.cmake)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (MSYS)
    message(FATAL_ERROR "Your should not use MSYS shell! Use MinGW64!") # Or MinGW32 if you know what you are doing.
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
set(ALL_TARGETS ${PROJECT_NAME} ${LIB_NAME})

# Include fixed CMake configuration
include(cmake/config.cmake)
 
