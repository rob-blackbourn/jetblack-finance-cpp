# jetblack-finance-cpp

Code for finance in C++.

## Status

This is work in progress.

## Overview

This is a scratchpad for developing financial programs in C++.

Support for c++23 is required.

## Building

This project uses the [Meson](https://mesonbuild.com/) build system.

### Prerequisites

- C++23 compatible compiler (e.g., clang 17+, gcc 12+)
- [Meson](https://mesonbuild.com/Getting-meson.html) build system
- [Ninja](https://ninja-build.org/) build tool (usually installed with Meson)
- OpenSSL development libraries

### Build Steps

```bash
# Configure the build
meson setup build

# Build all targets
meson compile -C build

# Run tests
meson test -C build

# Clean build artifacts
rm -rf build
```

### Build Outputs

- `build/src/librates.a` - Static library with financial instruments
- `build/tests/` - Test executables for all modules
