# CEVO — Command Engine

CEVO generates a C command interface from YAML and dispatches text commands to
application handlers. It suits embedded test consoles and diagnostic interfaces
that need bounded parsing without heap allocation.

Command names use precomputed 32-bit DJB2 hashes. The generator rejects collisions
within the configured command set and emits handler signature checks for GCC and
Clang. Runtime parsing checks argument counts, scalar types and numeric ranges.

## Build

Use CMake 3.20 or newer, a C compiler and Python with `ruamel.yaml`. Run these
commands from the checkout:

```bash
python3 -m venv .venv
.venv/bin/python -m pip install ruamel.yaml pytest
cmake -S . -B build -DPython_EXECUTABLE="$PWD/.venv/bin/python"
cmake --build build
```

The default build creates `libcevo.a` using `inputs/demo.yaml`. Its handler
prototypes live in `examples/demo.h`; an application provides the implementations
when linking the library. Building the archive does not require GoogleTest.

Generated headers and sources live in each build's `tools/apis/` directory.
Concurrent builds can select different YAML files without changing the source
checkout or overwriting another build's command table.

## Integrate an application

Define commands and include the header declaring their handlers:

```yaml
includes:
  - handlers.h
commands:
  - name: add
    handler: handle_add
    args:
      - type: u32
      - type: u32
```

```c
/* handlers.h */
#include <stdbool.h>
#include <stdint.h>
bool handle_add(uint32_t a, uint32_t b);
```

Configure `INPUT_YAML`, add CEVO to the application's CMake build, and supply the
handler header directory to the `cevo` target:

```cmake
set(INPUT_YAML "${CMAKE_CURRENT_SOURCE_DIR}/commands.yaml" CACHE FILEPATH "Command profile")
add_subdirectory(../cevo cevo_build)
target_include_directories(cevo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_link_libraries(app PRIVATE cevo)
```

Implement the handlers in your application and dispatch a null-terminated line:

```c
#include "ce_dispatch.h"
bool accepted = ce_dispatch_from_line("add 10 20");
```

Handlers return `true` on success. Dispatch returns `false` for an unknown command,
invalid arguments or a handler rejection. [demo-ce](https://github.com/nidops/demo-ce)
provides a Zephyr UART integration on the STM32WB55 Nucleo.

## Types and limits

YAML supports unsigned integers (`u8`, `u16`, `u32`, `u64`), signed integers
(`i8`, `i16`, `i32`, `i64`), `bool`, strings (`s` or `string`), and hexadecimal
byte buffers (`u8p`). Long integer names such as `uint32` are also accepted.

The current bounds in `inc/ce_types.h` are 255 input characters, seven arguments,
and a shared 64-byte scratch buffer for decoded byte arguments. Overlong lines
are rejected. String and byte pointers are borrowed for the duration of the
handler call; handlers must copy data they retain.

Byte pointers and scalar length arguments are independent. The application must
ensure a supplied length does not exceed the decoded data. Dispatch matches
command hashes, so this interface does not authenticate command names or callers.
Signature checks use GCC/Clang extensions in C99 mode. No MISRA compliance audit
or certification is claimed.

## Test

The generator suite exercises schema validation, C99 signature acceptance and
rejection, concurrent CMake generation, profile changes, and standalone build
and installation:

```bash
.venv/bin/python -m pytest tools/tests -q
```

The native C/C++ suite requires a C++17 compiler and downloads GoogleTest 1.17.0:

```bash
cmake -S . -B build-tests -DBUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug \
  -DPython_EXECUTABLE="$PWD/.venv/bin/python"
cmake --build build-tests --parallel
ctest --test-dir build-tests --output-on-failure
```

CI runs both suites and checks the default library build and installation.
The native suite covers dispatch, hashes, parser bounds and invalid inputs.

Licensed under Apache-2.0; see [LICENSE](LICENSE).
