Image conversion library with node binding

## Requirements
Library Specifcs:
- C++ 17 or higher
- CMake Version 3.9+
- Windows x64
- A few libraries are needed, refer to the Installation reference [HERE](#installation)

Node-Binding Specifics:
- npm 8.0+
- Node v16.0+
- node-gyp v9.0+
`$ npm install -g node-gyp`
Note though that `node-gyp` needs at least `Python v3.7` to be installed.
Windows: https://www.python.org/downloads/
For Ubuntu systems:

`sudo apt install software-properties-common -y`

`sudo add-apt-repository ppa:deadsnakes/ppa -y`

`sudo apt update`

`sudo apt install python3.8 -y`

Verify via `python3.8 --version`

Easiest way to make it accessable to `node-gyp` is to setup a `PYTHON` environment variable to the path of the `Python` executable.

## Installation
WIP
# Windows
- Use the VS Dev Console for the c++ library and another shell for the binding.
- Download, compile and install the following libraries: `Boost Header-only`,
`libpng`, `libjpeg-turbo`, `libwebp` and `zlib`. Header files should be copied into the
`src/headers` directory, the compiled libraries into the `src/libs/PRESET` one.

# Linux
- ZLIB: `sudo apt-get install zlib1g-dev`
- libpng: `sudo apt install libpng-dev`
- libjpeg: Follow the build and install instructions on https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/BUILDING.md

## Testing
The project includes test suites for both the c++ library and the node binding. The c++ tests are 
done via Google's GTest and can be triggered by going into the `tests` directory inside the build folder.
Note though that this is currently **Windows only**.

The binding includes the Jest test framework and testing can be triggered after the build process via
executing `npm test` or `npx jest` inside the `node` directory.

## Usage
WIP

## Troubleshooting
- Problem: Compiling the c++ library succeeds but compiling the binding with `node-gyp`throws linker errors. 
> Solution: Check whether both, the library and the binding, are build for the same architecture (32 vs 64 bit), the same build type  (Release etc.) and the same Toolset and runtime libraries.
If that doesnt help, try running the `node-gyp` commands in sequence: `node-gyp configure` and `node-gyp build`.
- Problem: I linked everything and every library has been found by cmake but it still throws linker errors on Linux.
> Solution: Try executing `ninja` with root privileges

## Attributions
All images used for unit testing have their source credited [HERE](ATTRIBUTIONS.md)
