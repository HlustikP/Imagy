const hashing = {
    testString: 'This is one fancy test string',
    targetString: 'c2cb4ebbd175f6bde5b4513d05f1506655ef360715689e1ae2c37e8928211ede',
}

const imageProcessing = {
    gif_decoding: {
        vanilla: {
            testFile: 'media/vanilla.gif',
            targetFile: 'media_output/vanilla.webp',
            targetHash: 'd885cddad9feb0dc399fb7f870f91a7a35e383f534d584c92740e6ac4134dace',
        },
        transparency: {
            testFile: 'media/transparency.gif',
            targetFile: 'media_output/transparency.webp',
            targetHash: '2e626050770b10f1c13ccaded958bf18080b75b46e0b8e7e4df08fb98e0b4d53',
        },
        interlacing: {
            testFile: 'media/interlacing.gif',
            targetFile: 'media_output/interlacing.webp',
            targetHash: '432626235308f8e33dade2b07b56ee11c9296265f17738b0458880ea9aa4e2a7',
        },
        local_color_tables: {
            testFile: 'media/lct.gif',
            targetFile: 'media_output/lct.webp',
            targetHash: '344e7d5b48c7dedef0e2da2395c448af048071cc1e90a620a29a598643df1258',
        },
    },
    image_conversion: {
        jpeg: {
            testFile: 'media/cat.jpg',
            targetFile: 'media_output/cat.jpg',
            targetHash: 'c5fcd03872f66d4d361f2da9241410e17224321ce563c88b355141b2d3030a07',
        },
        bmp: {
            testFile: 'media/cat.bmp',
            targetFile: 'media_output/cat.bmp',
            targetHash: 'cf776623e3bd2da2af8f86ad5195a263ec54336e8cafe1194f3f11e285b1208b',
        },
        png: {
            testFile: 'media/cat.png',
            targetFile: 'media_output/cat.png',
            targetHash: '6c18ae9a2cf06365304d246f09be7389406dbaa09d84199d5fa0786f9375a67c',
        },
    }
}

export const tests = {
    hashing,
    imageProcessing,
}
