import bindings from 'bindings';
import {createHash} from 'node:crypto';
import {readFileSync} from 'fs';

import {tests} from './test_ressources';

const imagy = require('../../index.js');

const hash_algorithm = 'sha256';
const digest = 'hex';

const hashAndTest = (targetFile, hash, targetHash) =>  {
    const contents = readFileSync(targetFile);

    hash.update(contents);

    return hash.digest(digest) === targetHash;
};

const getTestData = (data) => {
    return [data.testFile, data.targetFile, data.targetHash];
}

// Important to check, as it is fundamental to the functioning of the following test suites
describe('Test sha256 hashing implementation', () => {
    it('correctly hashes via sha256', () => {
        const hash = createHash(hash_algorithm);

        expect(hash.update(tests.hashing.testString).digest(digest)).toBe(tests.hashing.targetString);
    });
});

describe('Test gif decoding and (animated) webp encoding',() => {
    it('converts tiny gif with minimum features into webp', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.vanilla);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts tiny gif via alternate function signature', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.vanilla);
        const target = targetFile;

        imagy.convertSync(testFile, targetFile);

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts gif with transparency into webp', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.transparency);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts interlaced gif into webp', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.interlacing);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts gif with local color palettes into webp', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.local_color_tables);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
});

describe('Test image conversions', () => {
    it('de- and encodes jpeg files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.jpeg);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('de- and encodes bmp files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.bmp);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('de- and encodes png files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.png);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('de- and encodes tiff files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.tiff);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('decodes avif files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.avif);
        const target = targetFile;

        imagy.convertSync({
            'image': testFile,
            'outName': target,
        });

        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
});

describe('Test async', () => {
    it('returns a resolvable promise', async () => {
        expect.assertions(1);

        const [testFile, targetFile] = getTestData(tests.imageProcessing.image_conversion.png);

        await expect(imagy.convert({
            'image': testFile,
            'outName': targetFile,
        })).resolves.toBeTruthy();
    });
});

describe('Test async', () => {
    it('returns a resolvable promise via alternate function signature', async () => {
        expect.assertions(1);

        const [testFile, targetFile] = getTestData(tests.imageProcessing.image_conversion.png);

        await expect(imagy.convert(testFile, targetFile)).resolves.toBeTruthy();
    });
});

describe('Test Image class', () => {
    it('returns an Instance of class Image',() => {
        const img = new imagy.Image(tests.imageProcessing.image_conversion.jpeg.testFile);

        expect(img instanceof imagy.Image).toBeTruthy();
    });

    it('synchronously encodes image and writes it to a file',() => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.jpeg);
        const img = new imagy.Image(testFile);

        img.writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('asynchronously encodes image and writes it to a file',async () => {
        expect.assertions(1);

        const [testFile, targetFile] = getTestData(tests.imageProcessing.image_conversion.jpeg);
        const img = new imagy.Image(testFile);

        await expect(img.writeToFile(targetFile)).resolves.toBeTruthy();
    });

    it('synchronously rescales image',() => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.rescaling.bilinear);
        const img = new imagy.Image(testFile);

        img.rescaleSync(0, 4000).writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('synchronously rescales image via a non-default algorithm',() => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.rescaling.nearest_neighbour);
        const img = new imagy.Image(testFile);

        img.rescaleSync(0, 4000, imagy.Image.Algorithm.NEAREST_NEIGHBOUR).writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('synchronously flips image diagonally', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.flipping.diagonal);
        const img = new imagy.Image(testFile);

        img.flipDSync().writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('synchronously flips image along the horizontal axis', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.flipping.horizontal);
        const img = new imagy.Image(testFile);

        img.flipHSync().writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('synchronously flips image along the vertical axis', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.flipping.vertical);
        const img = new imagy.Image(testFile);

        img.flipVSync().writeToFileSync(targetFile);

        expect(hashAndTest(targetFile, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
});
