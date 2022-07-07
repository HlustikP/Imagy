Image conversion library with node binding

### Requirements
-------------
Library Specifcs:
- C++ 17 or higher
- CMake Version 3.0+ (Though 3.6+ is advised)

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

### Requirements
-------------
All images used for unit testing have their source credited [HERE](ATTRIBUTIONS.md)