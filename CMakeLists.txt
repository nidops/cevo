# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
project(cevo C)

# ------------------------------------------------------------------------------
# Language Standard
# ------------------------------------------------------------------------------
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Ensure toolchain is validated via static library (not executable)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------------------------
# Thumb Mode for ARM Targets
# ------------------------------------------------------------------------------
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_PROC_LC)
if(SYSTEM_PROC_LC MATCHES "arm" AND CMAKE_CROSSCOMPILING)
    message(STATUS "ARM cross-compilation detected, enabling -mthumb")
    set(EXTRA_CEVO_FLAGS "-mthumb")
else()
    message(STATUS "Native build or non-ARM target, skipping -mthumb")
endif()

# ------------------------------------------------------------------------------
# Code Generation (from YAML)
# ------------------------------------------------------------------------------
add_subdirectory(tools)

# ------------------------------------------------------------------------------
# Source Files
# ------------------------------------------------------------------------------
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

# Add generated files
if(NOT GENERATED_SRCS)
    message(FATAL_ERROR "GENERATED_SRCS is empty. Did 'generate_apis' run correctly?")
endif()
list(APPEND SRC_FILES ${GENERATED_SRCS})

# ------------------------------------------------------------------------------
# Static Library Target
# ------------------------------------------------------------------------------
add_library(cevo STATIC ${SRC_FILES})

# Target-specific compile flags
if(EXTRA_CEVO_FLAGS)
    target_compile_options(cevo PRIVATE ${EXTRA_CEVO_FLAGS})
endif()

# Only add own includes; caller sets its own include paths and flags
target_include_directories(cevo PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/apis
)

# Ensure generated files are built before cevo
add_dependencies(cevo generate_apis)

# ------------------------------------------------------------------------------
# Unit Tests
# ------------------------------------------------------------------------------
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
    enable_testing()
    add_subdirectory(unit_test)

    # Add test-only include dirs (conditionally)
    target_include_directories(cevo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/unit_test
    )

    # Define unit test build macro
    target_compile_definitions(cevo PRIVATE UNIT_TEST)
endif()

# ------------------------------------------------------------------------------
# Install (For reuse by other projects)
# ------------------------------------------------------------------------------
install(TARGETS cevo
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY inc/ DESTINATION include)
install(DIRECTORY apis/ DESTINATION include)
