import bindings from 'bindings';
const imagy = bindings('imagy');
import { createHash } from 'node:crypto';
import { createReadStream } from 'fs';

import { tests } from './test_ressources.js';

test('two plus two is four', () => {
    expect(2 + 2).toBe(4);
});

test('correctly hashes via sha256', () => {
    const hash = createHash('sha256');
    expect(hash.update(tests.hashing.test_string).digest('hex')).toEqual(tests.hashing.target_string);
});

test('correctly decodes interlaced gif', function () {
    const target = tests.imageProcessing.gif_decoding.interlacing.target_file;
    console.log(imagy.convert({
        'image': tests.imageProcessing.gif_decoding.interlacing.test_file,
        'outName': target,
    }));
    const hash = createHash('sha256');
    const input = createReadStream(target);
    input.on('readable', () => {
        const data = input.read();
        if (data)
            hash.update(data);
        else {
           expect(hash.digest('hex')).toEqual(tests.imageProcessing.gif_decoding.interlacing.target_hash));
        }
    });
});