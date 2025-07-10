# SPDX-License-Identifier: Apache-2.0

# --------------------------------------------------------------------
# Respect BUILD_TESTS toggle from root
# --------------------------------------------------------------------
if(NOT BUILD_TESTS)
    return()
endif()

# --------------------------------------------------------------------
# Enable C++ for GoogleTest
# --------------------------------------------------------------------
enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------------------------
# Fetch GoogleTest
# --------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --------------------------------------------------------------------
# Unit Test Sources
# --------------------------------------------------------------------
file(GLOB UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# --------------------------------------------------------------------
# Enable Address Sanitizer to Catch Crashes (Debug only)
# --------------------------------------------------------------------
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -g")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fsanitize=address")

# --------------------------------------------------------------------
# Executable for Tests
# --------------------------------------------------------------------
message(STATUS "Looking for generated files in: ${CMAKE_SOURCE_DIR}/apis")
file(GLOB GENERATED_FILES ${CMAKE_SOURCE_DIR}/apis/*.c)
message(STATUS "Generated files: ${GENERATED_FILES}")

add_executable(test_cevo ${UNIT_TEST_SOURCES})

# Ensure generated files are built before the test
add_dependencies(test_cevo generate_apis)

# --------------------------------------------------------------------
# Include Paths for Testing
# --------------------------------------------------------------------
target_include_directories(test_cevo
    PRIVATE
    ${CMAKE_SOURCE_DIR}/inc        # Public API
    ${CMAKE_SOURCE_DIR}/apis       # Generated API
    ${CMAKE_SOURCE_DIR}/src        # Internal headers (test-only)
    ${CMAKE_CURRENT_SOURCE_DIR}    # Local test headers/mocks
)

# --------------------------------------------------------------------
# Link Libraries
# --------------------------------------------------------------------
target_link_libraries(test_cevo
    PRIVATE
    gtest_main
    cevo
)

# --------------------------------------------------------------------
# Register Test
# --------------------------------------------------------------------
add_test(NAME test_cevo COMMAND test_cevo)

# --------------------------------------------------------------------
# Compile Definitions for Unit Test
# --------------------------------------------------------------------
target_compile_definitions(test_cevo PRIVATE UNIT_TEST)
