Image conversion and manipulation library with node binding

Requires either x64 Windows 10 (might run on Windows Server, not tested) or x64 Debian-based system with support for glibc 2.31 or higher.
Check your glibc version with:
```bash
ls /lib/x86_64-linux-gnu/ | grep -E '^libc-[0-9]\.[0-9]{1,2}\.so$'
```

## Features
### Supported Image Formats:
- In all directions: BMP, PNG, JPEG, WEBP, TIFF
- In one direction only: GIF -> animated WEBP
### Image Manipulation:
- Resizing (unanimated images only)
- Flipping (unanimated images only)

Imagy is a library that uses a mixture of existing libraries (Boost::GIL, libpng, libjpeg, libwebp) and self-built decoders to provide
an easy-to-use library interface for image conversion and manipulation. All images are internally
stored as a (decoded) pixel array to allow fast operations on the data. De- and encoding are only
ever done upon FileIO.

Additionally, the en- and decoding routines for animated images make use of multithreading to
speed up the process.

## Usage
This library comes with pre-built binaries. You can install the package with
```
npm i imagy
```

Inlude the package into your js application with:
```js
const imagy = require('imagy');
```

For quick image conversions the library implements the `convert` and `convertSync` functions:
```js
const imagy = require('imagy');

imagy.convertSync({
            'image': 'path/to/file.jpg',
            'outName': 'path/to/target.png',
        });
```
```js
const imagy = require('imagy');

// Converts asynchronously and returns a promise
await imagy.convert({
            'image': 'path/to/file.jpg',
            'outName': 'path/to/target.png',
        });
```
These functions take an Object with two properties as an argument. The first `image` is the path to the file to be converted
and the second `outName` is the target. The function tries to infer the input's image type from the **file extension**,
or if none is found or the one used is unknown, tries to parse the **file header** to get information on the image type. 
The `convert` function returns a promise that, if resolved, returns
and Object with `finished`, `error` and `img`(path to target) keys.

You can also use the `Image` class:
```js
const imagy = require('imagy');

const img = new imagy.Image('path/to/input/file.png');

img.writeToFileSync('some/image.jpg');

// async variant
await img.writeToFile('another/image.bmp');
```
**Note** though that this way the underlying image data are only freed once the `Image` instance
gets garbage collected, which triggers the underlying cpp-class's destructor.

Image objects also have access to the `rescale` and `rescaleSync` methods, which return the 
calling object, so you might use them as follows:
```js
const imagy = require('imagy');
const img = new imagy.Image('path/to/input/file.png');

//          (HEIGHT, WIDTH)
img.rescaleSync(0, 4000).writeToFileSync(targetFile);
```
A `0` argument here indicates, that the dimension should be calculated in such a way that the image
proportions are retained.

See the [the according doc file](docs/BINDING.md) for a full documentation.

## Build Requirements
This package has been built and tested on Windows 10, Kali Linux 2021.4 x64
and Ubuntu 20.04.4 LTS via WSL2.

Library Specifcs:
- C++ 17 or higher
- Ninja Build System v1.11+
- msvc (windows) or gcc (linux)
- CMake Version 3.9+
- Windows x64 or a debian-based Linux distro x64
- A few libraries are needed, refer to the Installation reference [HERE](#installation)

Node-Binding Specifics:
- npm 8.0+
- Node v14.0+
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
executing `npm test` or `npx jest` inside the root directory.

## Troubleshooting
- Problem: Compiling the c++ library succeeds but compiling the binding with `node-gyp`throws linker errors. 
> Solution: Check whether both, the library and the binding, are build for the same architecture (32 vs 64 bit), the same build type  (Release etc.) and the same Toolset and runtime libraries.
If that doesnt help, try running the `node-gyp` commands in sequence: `node-gyp configure` and `node-gyp build`.
- Problem: I linked everything and every library has been found by cmake but it still throws linker errors on Linux.
> Solution: Try executing `ninja` with root privileges

## TODO:
- Expand the README
- Implement more image manipulation options
- Support more formats
- Expose the libweb configs api to enable finetuning of animated images

## Attributions
All images used for unit testing have their source credited [HERE](tests/media/ATTRIBUTIONS.md)
(needs the related submodule to be installed).
