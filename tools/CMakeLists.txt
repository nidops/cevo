# SPDX-License-Identifier: Apache-2.0

# ------------------------------------------------------------------------------
# Python Interpreter (Required)
# ------------------------------------------------------------------------------
find_package(Python COMPONENTS Interpreter REQUIRED)

# ------------------------------------------------------------------------------
# Validate Python & Required Packages
# ------------------------------------------------------------------------------
if(NOT DEFINED PYTHON_EXECUTABLE)
    set(PYTHON_EXECUTABLE ${Python_EXECUTABLE} CACHE FILEPATH "Python interpreter to run generator")
endif()

if(NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "❌ No Python interpreter found.")
endif()

# Validate Python version ≥ 3.6
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import sys; assert sys.version_info >= (3,6)"
    RESULT_VARIABLE PYTHON_VERSION_OK
)
if(NOT PYTHON_VERSION_OK EQUAL 0)
    message(FATAL_ERROR "❌ Python >= 3.6 is required to run generate_apis.py")
endif()


# List of required Python modules
set(REQUIRED_PYTHON_MODULES "ruamel.yaml")
set(MISSING_MODULES "")

# Check availability of each module
foreach(MOD IN LISTS REQUIRED_PYTHON_MODULES)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import ${MOD}"
        RESULT_VARIABLE MOD_RESULT
        OUTPUT_QUIET ERROR_QUIET
    )
    if(NOT MOD_RESULT EQUAL 0)
        list(APPEND MISSING_MODULES ${MOD})
    endif()
endforeach()

# If any missing, run ensure_venv.sh
if(MISSING_MODULES)
    message(WARNING "🔍 Missing Python modules: ${MISSING_MODULES}")
    message(STATUS "🚧 Running ensure_venv.sh to create .venv and install dependencies...")

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E env bash ${CMAKE_CURRENT_SOURCE_DIR}/ensure_venv.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE ENV_SETUP_RESULT
    )

    if(NOT ENV_SETUP_RESULT EQUAL 0)
        message(FATAL_ERROR "❌ Failed to set up Python environment via ensure_venv.sh")
    endif()

    # Override Python executable to use .venv Python
    set(PYTHON_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/../.venv/bin/python" CACHE FILEPATH "Python interpreter from .venv" FORCE)
    message(STATUS "🐍 Using virtualenv Python: ${PYTHON_EXECUTABLE}")
endif()

# ------------------------------------------------------------------------------
# Input YAML and Output Targets
# ------------------------------------------------------------------------------

# Allow overriding via -DINPUT_YAML=... and cache it for visibility
set(INPUT_YAML "${CMAKE_CURRENT_SOURCE_DIR}/../inputs/demo.yaml"
    CACHE FILEPATH "YAML input file for API generation"
)

# Validate YAML exists
if(NOT EXISTS ${INPUT_YAML})
    message(FATAL_ERROR "❌ Input YAML file not found: ${INPUT_YAML}")
endif()
message(STATUS "📄 INPUT_YAML: ${INPUT_YAML}")

# Ensure apis/ directory exists
set(API_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../apis)
file(MAKE_DIRECTORY ${API_DIR})

# Output paths
set(GENERATED_HEADER   ${API_DIR}/ce_command_ids.h)
set(GENERATED_SIGTABLE ${API_DIR}/ce_table.c)
set(GENERATED_INVOKE   ${API_DIR}/ce_invoke_handler.c)
set(GENERATOR_SCRIPT   ${CMAKE_CURRENT_SOURCE_DIR}/generate_apis.py)

# ------------------------------------------------------------------------------
# Code Generation Command
# ------------------------------------------------------------------------------
add_custom_command(
    OUTPUT ${GENERATED_HEADER} ${GENERATED_SIGTABLE} ${GENERATED_INVOKE}
    COMMAND ${CMAKE_COMMAND} -E echo "🐍 Running YAML-to-C generator..."
    COMMAND ${CMAKE_COMMAND} -E env PYTHONUNBUFFERED=1
            ${PYTHON_EXECUTABLE} ${GENERATOR_SCRIPT}
            --input ${INPUT_YAML}
            --header ${GENERATED_HEADER}
            --sigtable ${GENERATED_SIGTABLE}
            --invoke ${GENERATED_INVOKE}
    DEPENDS ${INPUT_YAML} ${GENERATOR_SCRIPT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "🛠 Generating APIs from YAML..."
    VERBATIM
)

# Mark outputs as GENERATED for IDEs
set_source_files_properties(${GENERATED_HEADER} PROPERTIES GENERATED TRUE)
set_source_files_properties(${GENERATED_SIGTABLE} PROPERTIES GENERATED TRUE)
set_source_files_properties(${GENERATED_INVOKE} PROPERTIES GENERATED TRUE)

# Propagate to root
set(GENERATED_SRCS ${GENERATED_SIGTABLE} ${GENERATED_INVOKE} PARENT_SCOPE)
set(GENERATED_HDRS ${GENERATED_HEADER} PARENT_SCOPE)

# ------------------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------------------
# Custom target to trigger codegen
add_custom_target(generate_apis
    DEPENDS ${GENERATED_HEADER} ${GENERATED_SIGTABLE} ${GENERATED_INVOKE}
)

# Optional clean target
add_custom_target(clean_apis
    COMMAND ${CMAKE_COMMAND} -E remove
            ${GENERATED_HEADER}
            ${GENERATED_SIGTABLE}
            ${GENERATED_INVOKE}
    COMMENT "🧹 Cleaning generated API files"
)
