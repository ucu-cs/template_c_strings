cmake_minimum_required(VERSION 3.15)

# CHANGE YOUR PROJECT NAME
set(PROJECT_NAME test)

project(${PROJECT_NAME} C)

# Options
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)
option(ENABLE_SANITIZERS "Use sanitizers to detect errors." OFF)
option(ENABLE_CONAN "Use Conan as a package manager." OFF)
option(ENABLE_PVS_STUDIO "Check using command-line PVS-Studio." OFF)
# ENABLE_SANITIZERS is the option for the test builds! Do not use it in the production builds.

## PVS Studio
if(ENABLE_PVS_STUDIO)
    include(cmake/PVS-Studio.cmake)
    SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Warnings
if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror -Werror=vla)
endif()

# Compile program to ./bin directory, not to ./build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Include fixed CMake configuration
include(cmake/config.cmake)

# Include headers
include_directories(inc)

# Project source compilation
add_executable(${PROJECT_NAME} src/main.c)

# Sanitizers configuration
if (ENABLE_SANITIZERS)
    set(SANITIZE_UNDEFINED ON)
    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
    set(SANITIZE_MEMORY ON)
    #    set(SANITIZE_ADDRESS ON)
    #    set(SANITIZE_THREAD ON)
    find_package(Sanitizers)

    # CMake targets to which apply sanitizers
    add_sanitizers(${PROJECT_NAME})
endif ()
# It is OK that some compilers do not support all of the sanitizers -- messages like:
# # CMake Warning at cmake/sanitizers/sanitize-helpers.cmake:150 (message):
# # UndefinedBehaviorSanitizer is not available for compiler.  Targets using
# #  this compiler will be compiled without UndefinedBehaviorSanitizer.
# are OK, though test using at least one compiler supporting all of them -- may be clang or GCC-10.

#####################################
if(ENABLE_PVS_STUDIO)
# create own target for library
pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
        ANALYZE ${PROJECT_NAME}
        OUTPUT FORMAT errorfile
        MODE GA:1,2,3 OP:1,2,3 GA:1,2,3 OP:1,2,3 64:1,2,3 CS:1,2,3 MISRA:1,2,3
        LOG target_${PROJECT_NAME}.err)
else()
    message("Consider checking using PVS-Studio with `cmake -DENABLE_PVS_STUDIO=ON <.....>` or Windows GUI application.")
endif()
#####################################
