const hashing = {
    test_string: 'This is one fancy test string',
    target_string: 'c2cb4ebbd175f6bde5b4513d05f1506655ef360715689e1ae2c37e8928211ede',
}

const imageProcessing = {
    gif_decoding: {
        vanilla: {
            test_file: 'media/vanilla.gif',
            target_file: 'media_output/vanilla.webp',
            target_hash: '',
        },
        interlacing: {
            test_file: 'media/interlacing.gif',
            target_file: 'media_output/interlacing.webp',
            target_hash: '432626235308f8e33dade2b07b56ee11c9296265f17738b0458880ea9aa4e2a7',
        },
        local_color_tables: {
            test_file: 'media/lct.gif',
            target_file: 'media_output/lct.webp',
            target_hash: '',
        },
    },
}

export const tests = {
    hashing,
    imageProcessing,
}
