import { resolve } from 'path';

const hashing = {
    testString: 'This is one fancy test string',
    targetString: 'c2cb4ebbd175f6bde5b4513d05f1506655ef360715689e1ae2c37e8928211ede',
}

const imageProcessing = {
    gif_decoding: {
        vanilla: {
            testFile: resolve(__dirname ,'../media/vanilla.gif'),
            targetFile: resolve(__dirname, '../media_output/vanilla.webp'),
            targetHash: 'd885cddad9feb0dc399fb7f870f91a7a35e383f534d584c92740e6ac4134dace',
        },
        transparency: {
            testFile: resolve(__dirname, '../media/transparency.gif'),
            targetFile: resolve(__dirname, '../media_output/transparency.webp'),
            targetHash: '2e626050770b10f1c13ccaded958bf18080b75b46e0b8e7e4df08fb98e0b4d53',
        },
        interlacing: {
            testFile: resolve(__dirname, '../media/interlacing.gif'),
            targetFile: resolve(__dirname, '../media_output/interlacing.webp'),
            targetHash: '432626235308f8e33dade2b07b56ee11c9296265f17738b0458880ea9aa4e2a7',
        },
        local_color_tables: {
            testFile: resolve(__dirname, '../media/lct.gif'),
            targetFile: resolve(__dirname, '../media_output/lct.webp'),
            targetHash: '344e7d5b48c7dedef0e2da2395c448af048071cc1e90a620a29a598643df1258',
        },
    },
    image_conversion: {
        jpeg: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: 'c5fcd03872f66d4d361f2da9241410e17224321ce563c88b355141b2d3030a07',
        },
        bmp: {
            testFile: resolve(__dirname, '../media/bmp'),
            targetFile: resolve(__dirname, '../media_output/cat.bmp'),
            targetHash: '145eba200a44556f626ba4cbf60c4218dedbe44ef66573beb30532f19024ecd9',
        },
        png: {
            testFile: resolve(__dirname, '../media/cat.png'),
            targetFile: resolve(__dirname, '../media_output/cat.png'),
            targetHash: '6c18ae9a2cf06365304d246f09be7389406dbaa09d84199d5fa0786f9375a67c',
        },
        tiff: {
            testFile: resolve(__dirname, '../media/cat.tiff'),
            targetFile: resolve(__dirname, '../media_output/cat.tiff'),
            targetHash: '1b68f07013f8c6c1768d1c63cd51c2d0d0e9e404183cfd9d6bb77d315c590284',
        },
    },
    rescaling: {
        bilinear: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: '7a12433ce036c1e798fcb514c4adbb4af2db2afd7341aa9a9efab719d4fe6780',
        },
        nearest_neighbour: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: 'ec18721518a4b58cedd98089f5f607ad652c9f6af2cec9d921c534e9d69d91e0',
        },
    },
    flipping: {
        diagonal: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: '66d36ff192b7eea30f5d0725efba0e6f74e08a942d84de7453507d005727a211',
        },
        horizontal: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: '3598e867e4afa5093a602d0f4ce721ff9626fa391a46b0ad47b121722dfec1f1',
        },
        vertical: {
            testFile: resolve(__dirname, '../media/cat.jpg'),
            targetFile: resolve(__dirname, '../media_output/cat.jpg'),
            targetHash: 'f31fc7bfafba707dfffb814f3aeaa36de99ca9b10916fc005599987e9591df30',
        }
    }
}

export const tests = {
    hashing,
    imageProcessing,
}
