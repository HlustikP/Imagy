import bindings from 'bindings';
import {createHash} from 'node:crypto';
import {readFileSync} from 'fs';

import {tests} from './test_ressources.js';

const imagy = bindings('imagy');

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

describe('Test sha256 hashing implementation, as it is fundamental to the functioning of the following test suites', () => {
    it('correctly hashes via sha256', () => {
        const hash = createHash(hash_algorithm);
        expect(hash.update(tests.hashing.testString).digest(digest)).toBe(tests.hashing.targetString);
    });
});

describe('Test gif decoding and (animated) webp encoding',() => {
    it('converts tiny gif with minimum features into webp', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.vanilla);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts gif with transparency into webp', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.transparency);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts interlaced gif into webp', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.interlacing);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });

    it('converts gif with local color palettes into webp', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.gif_decoding.local_color_tables);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
});

describe('Test image conversions', () => {
    it('de- and encodes jpeg files', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.jpeg);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
    it('de- and encodes bmp files', function () {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.bmp);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        // hashAndTest(target, createHash(hash_algorithm), targetHash);
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
    it('de- and encodes png files', () => {
        const [testFile, targetFile, targetHash] = getTestData(tests.imageProcessing.image_conversion.png);
        const target = targetFile;
        imagy.convert({
            'image': testFile,
            'outName': target,
        });
        expect(hashAndTest(target, createHash(hash_algorithm), targetHash)).toBeTruthy();
    });
});

describe('Test async', function () {
    it('returns a resolvable promise', async () => {
        expect.assertions(1);
        const [testFile, targetFile] = getTestData(tests.imageProcessing.image_conversion.png);
        await expect(imagy.convertAsync({
            'image': testFile,
            'outName': targetFile,
        })).resolves.toBeTruthy();
    });
});
