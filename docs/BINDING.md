# Documentation for the imagy node binding

# Table of Contents
1. [Quick Conversion](#quick-conversion)
2. [Image Class](#the-image-class)
    1. [Construction](#construction)
    2. [Conversion](#conversion)
    3. [Resizing](#resizing)
    4. [Flipping](#flipping)

## 1. Quick Conversion
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
and the second `outName` is the target. The function infers the image type from the **file extension**,
which is therefore needed. The `convert` function returns a promise that, if resolved, returns
and Object with `finished`, `error` and `img`(path to target) keys.

The binding also implements the `quickConvert` and `quickConvertSync` functions, which are just
aliases of the beforementioned two functions.

## 2. The Image Class
The image class enables you to load in images, perform manipulations on them and then write
them to a target location as a chosen image type. All images are internally stored as a raw
(decoded) pixel array to allow fast operations on the data. De- and encoding are only
ever done upon FileIO.

Additionally, the en- and decoding routines for animated images make use of multithreading to
speed up the process.

### 2.1. Construction
The class constructor takes 1 argument, which is a string with the path to an image file to be loaded:
```js
const imagy = require('imagy');

const img = new imagy.Image('path/to/input/file.png');
```
The constructor will determine the image type via the file extensions, which means it will throw
an error if no or an invalid extension is detected (see [README](../README.md) for a list of supported
file types).

### 2.2 Conversion
Images are automatically decoded when read. Encoding happens upon writing the damage data to
a given destination. This can be done either `synchronous` are `async`:
```js
const imagy = require('imagy');

const img = new imagy.Image('path/to/input/file.png');

img.writeToFileSync('some/image.jpg');

// async variant
await img.writeToFile('another/image.bmp');
```
**Note** though that this way the underlying image data are only freed once the `Image` instance
gets garbage collected, which triggers the underlying cpp-class's destructor.

### 2.3 Resizing
Image objects also have access to the `rescale`(async) and `rescaleSync` methods, which return the 
calling object, so you might use them as follows:
```js
const imagy = require('imagy');
const img = new imagy.Image('path/to/input/file.png');

//          (HEIGHT, WIDTH)
img.rescaleSync(0, 4000).writeToFileSync(targetFile);
```
A `0` argument here indicates, that the dimension should be calculated in such a way that the image
proportions are retained.
As for the algorithm used for resizing, the methods above use bilinear interpolation as a default.
You can however choose from a list of given algorithms:
```js
const imagy = require('imagy');
const img = new imagy.Image('path/to/input/file.png');

// returns { NEAREST_NEIGHBOUR: 0, BILINEAR: 1 }
const algorithms = imagy.Image.Algorithm;
// Use non-default resizing algorithm
img.rescaleSync(0, 4000, algorithms.NEAREST_NEIGHBOUR).writeToFileSync(targetFile);
```
The `rescale` and `rescaleSync` methods take an optional third argument, which should be a
member of the `Image.Algorithm` static Object.
Bilinear interpolation (default) results in softer edges but is slower, whereas nearest-neighbour
is very fast but leads to more artifacts.

### 2.4 Flipping
Images can also be flipped along the horizontal, vertical or diagonal (equivalent to a vertical +
horizontal flip) axis:
```js
const imagy = require('imagy');
const img = new imagy.Image('path/to/input/file.png');

// diagonal
img.flipDSync().writeToFileSync('targetfile.jpg');
// vertical
img.flipVSync().writeToFileSync('targetfile.jpg');
// horizontal
img.flipHSync().writeToFileSync('targetfile.jpg');
```

There is also the `flipSync` method that takes 1 argument, which is a one-letter-string:
```js
const imagy = require('imagy');
const img = new imagy.Image('path/to/input/file.png');

// diagonal
img.flipSync('D').writeToFileSync('targetfile.jpg');
// vertical
img.flipSync('V').writeToFileSync('targetfile.jpg');
// horizontal
img.flipSync('H').writeToFileSync('targetfile.jpg');
```
