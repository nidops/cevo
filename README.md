# CEVO - Command Engine

CEVO is a lightweight command parser.
It turns simple text commands into safe, validated function calls, with strict type checking, zero dynamic memory, and fast dispatching based on precomputed hashes.
Built for test consoles, debug shells, field diagnostics, and scriptable command interfaces, all wrapped in a MISRA-C-friendly codebase (with just a dash of rebellion). 😅

## Features

* Runtime command parsing from text input with tokenization and type validation
* Supports multiple argument types (uint8, int64, strings, boolean, binary arrays)
* Hash-based command dispatch
* Zero dynamic memory allocation
* Drop-in C integration, no runtime registry or magic
* Unit tests using GoogleTest
* MISRA-C friendly codebase, optimized for clarity and maintainability

## Project Structure

```
├── build.sh            # Build helper script
├── CMakeLists.txt      # Root CMake configuration
├── Makefile
├── LICENSE             # Apache-2.0
├── README.md
├── inc/                # Public headers
│ ├── ce_dispatch.h
│ ├── ce_hash.h
│ ├── ce_invoke_handler.h
│ ├── ce_table.h
│ └── ce_types.h
├── src/                # Source code implementations
│ ├── ce_dispatch.c
│ ├── ce_hash.c
│ ├── logger.c
│ ├── logger.h
│ ├── parser_utils.c
│ └── parser_utils.h
├── build/tools/apis/   # Generated files, isolated in each CMake build
├── inputs/             # Example YAML command definitions
│ └── demo.yaml
├── tools/              # Python scripts for code generation and testing
│ ├── cli.py
│ ├── generate_apis.py
│ ├── gen_header.py
│ ├── gen_invoke_handler.py
│ ├── gen_signature_table.py
│ ├── main.py
│ ├── parser.py
│ ├── tests/          # Python unit tests for tooling
│ │ ├── test_generator.py
│ │ ├── test_parser.py
│ │ └── test_utils.py
│ └── utils.py
└── unit_test/          # Main project unit tests with GoogleTest
    ├── ce_dispatch_test_api.h
    ├── CMakeLists.txt
    ├── demo.h
    ├── test_dispatch.cpp
    ├── test_hash.cpp
    └── test_parser_utils.cpp
```

## Getting Started

### Building

Use your preferred C build system (Make, CMake, etc.). Example with CMake:

```bash
mkdir build && cd build
cmake ..
make
```

Or run the included `build.sh` helper script.

### Usage

Include the public header:

```c
#include "ce_dispatch.h"
```

Then dispatch commands by calling:

```c
ce_dispatch_from_line("command_name arg1 arg2 ...");
```

Handlers are registered via signatures with hashed command names.

### Code Generation

* Command definitions are written in YAML files (e.g., `inputs/demo.yaml`).
* Python scripts in `tools/` generate C headers and source files based on YAML.
* Example:

```yaml
# inputs/demo.yaml
includes:
  - demo.h

commands:
  - name: cat_string
    handler: demo_cat_string
    args:
      - type: string

  - name: cat_bytes
    handler: demo_cat_bytes
    args:
      - type: u8p
      - type: u8
```

```bash
cd tools
make venv
source ../.venv/bin/activate
python3 generate_apis.py -i ../inputs/demo.yaml
```

Or just run `python3 generate_apis.py --help` and enjoy.

### Running Unit Tests

* Unit tests are located in `unit_test/` and use GoogleTest.
* Build tests via CMake:

```bash
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON
make
ctest
```

Or

```bash
make
make run_tests
```

## Supported Argument Types

* UINT8, UINT16, UINT32, UINT64
* INT8, INT16, INT32, INT64
* BOOLEAN (true/false)
* STRING (parsed into bounded internal buffer)
* Binary buffers via hex string arguments

## Contributing

Contributions and issues are welcome!
