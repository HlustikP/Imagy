Image conversion library with node binding

## Requirements
Library Specifcs:
- C++ 17 or higher
- CMake Version 3.0+ (Though 3.6+ is advised)
- Windows x64 (Linux compatibility is currently WIP)
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

# Linux
- ZLIB: `sudo apt-get install zlib1g-dev`
- libpng: `sudo apt install libpng-dev`
- libjpeg: Follow the build and install instructions on https://github.com/libjpeg-turbo/libjpeg-turbo/blob/main/BUILDING.md

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
