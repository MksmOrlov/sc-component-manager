# Quick Start for Contributors

This guide provides short information for developers to start to work with sc-component-manager quickly. You can always learn more about the sc-component-manager's [build system](build_system.md).

## Install project

Install sc-component-manager via git:

```sh
git clone https://github.com/ostis-ai/sc-component-manager
cd sc-component-manager
git submodule update --init --recursive
```

## Check CMake

Install pipx first using [**pipx installation guide**](https://pipx.pypa.io/stable/installation/) if not already installed.

Ensure you are using **CMake version 3.24** or newer. Verify your version with:

```sh
cmake --version
```

To upgrade CMake, run:
  
```sh
# Use pipx to install cmake if not already installed
pipx install cmake
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

Install Ninja generator for CMake, to use CMake presets:

```sh
# Use pipx to install ninja if not already installed
pipx install ninja
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

## Start develop sc-component-manager with Conan

### Install Conan

Install Conan, to build sc-component-manager dependencies with Conan-provided dependencies:

```sh
# Use pipx to install conan if not already installed
pipx install conan
pipx ensurepath
# relaunch your shell after installation
exec $SHELL
```

### Add Conan remote repository

Add Conan remote repository with the specified URL:

```sh
conan remote add ostis-ai https://conan.ostis.net/artifactory/api/conan/ostis-ai-library
```

### Use sc-component-manager in Debug

#### Install dependencies with Conan

sc-component-manager is an extension to sc-machine, so sc-machine is main dependency for sc-component-manager. To install it, run in the root of the project:

```sh
conan install . -s build_type=Debug --build=missing
```

#### Build sc-component-manager in Debug

To build sc-component-manager in debug mode using Conan-provided dependencies, run:

```sh
# debug build type
cmake --preset debug-conan
cmake --build --preset debug
```

Note: By default, configure preset `debug` enables building sc-component-manager tests.

#### Run sc-component-manager tests in Debug

After that, you can go to `build/Debug` and run tests via `ctest`:

```sh
cd build/Debug
ctest -V
```

You can also check code formatting. To learn more, go to the [CMake flags](cmake_flags.md) page.

### Use sc-component-manager in Release

#### Install dependencies with Conan

To install it, run in the root of the project:

```sh
conan install . -s build_type=Release --build=missing
```

#### Build sc-component-manager in Release

To build sc-component-manager in release mode using Conan-provided dependencies, run:

```sh
# release build type without tests
cmake --preset release-conan
cmake --build --preset release
```

To build sc-component-manager with tests in release mode using Conan-provided dependencies, run:

```sh
# release build type with tests
cmake --preset release-with-tests-conan
cmake --build --preset release
```

#### Run sc-component-manager tests in Release

After that, you can run tests:

```sh
cd build/Release
ctest -V
```

You can also check code formatting. To learn more, go to the [CMake flags](cmake_flags.md) page.

### Run sc-component-manager

Before launching sc-component-manager, [extract sc-machine from GitHub Releases](https://ostis-ai.github.io/sc-machine/quick_start/#github-releases) or [build it](https://ostis-ai.github.io/sc-machine/build/quick_start/).

To launch sc-component-manager, run:

```sh
./path/to/sc-machine/binary -s path/to/kb.bin \
    -e "sc-component-manager/<Debug|Release>/lib/extensions;path/to/sc-machine/lib/extensions"
# if several paths to extensions are provided then they should be separated 
# by semicolon and wrapped in double quotes
```
