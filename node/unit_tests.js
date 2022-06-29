import bindings from 'bindings';
const addon = bindings('imagy');
import { createHash } from 'node:crypto';
import assert from 'assert';
import { createReadStream } from 'fs';
import { describe, it } from 'mocha';

import { tests } from './test_ressources.js';

describe('Hash', () => {
    it('correctly hashes via sha256', () => {
        const hash = createHash('sha256');
        assert.equal(hash.update(tests.hashing.test_string).digest('hex'), tests.hashing.target_string);
    });
});

describe('Gif Decoding', function () {
    it('correctly decodes interlaced gif', function () {
        this.slow(800);
        addon.convert("navmap.gif");
        const hash = createHash('sha256');
        const input = createReadStream(tests.imageProcessing.gif_decoding.target_file);
        input.on('readable', () => {
            const data = input.read();
            if (data)
                hash.update(data);
            else {
                assert.equal(hash.digest('hex'), tests.imageProcessing.gif_decoding.target_hash);
            }
        });
    });
});
