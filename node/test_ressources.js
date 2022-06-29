const hashing = {
    test_string: 'This is one fancy test string',
    target_string: 'c2cb4ebbd175f6bde5b4513d05f1506655ef360715689e1ae2c37e8928211ede',
}

const imageProcessing = {
    gif_decoding: {
        test_file: 'media/interlacing.gif',
        target_file: 'media_output/interlacing.webp',
        target_hash: '432626235308f8e33dade2b07b56ee11c9296265f17738b0458880ea9aa4e2a7',
    },
}

export const tests = {
    hashing,
    imageProcessing,
}
