{
  'targets': [
    {
      'target_name': 'imagy',
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'cflags_cc': [ '-std=gnu++17' ],
      'sources': [
        'src/binding/imagy.cpp',
        'src/binding/async_workers.hpp',
        'src/binding/convert.h',
        'src/binding/convert.cpp',
        'src/binding/image.cpp',
        'src/binding/image.h'
      ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
		    'src/headers'
     ],
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
	  'conditions': [ ["OS=='win'",
		{
      'link_settings': {
			  'libraries': [
			    'image',
			    'utils',
			    'libwebp',
			    'libwebpmux',
			    'libwebpdemux',
			    'jpeg-static',
			    'libpng16',
			    'zlib'
			  ],
	  },
			'configurations': {
				'Release': {
          'library_dirs': [
            'src/libs/x64-release',
			    ],
					'msvs_settings': {
						'VCCLCompilerTool': {
							'RuntimeLibrary': 0,
							'ExceptionHandling': 1,
						    'AdditionalOptions': [
                  '-std:c++17',
                  '/GR',
                  '/EHsc',
						    ],
						}
					}
				}
			}
		}
		],
    ["OS=='linux'",
      {
        "libraries": [
          "../../src/libs/linux-release/libutils.a",
          "../../src/libs/linux-release/libimage.a",
          "/opt/libjpeg-turbo/lib64/libjpeg.so",
          "/usr/local/lib/libwebp.so",
          "/usr/local/lib/libwebpmux.so",
          "/usr/local/lib/libwebpdemux.so",
          "/usr/lib/x86_64-linux-gnu/libpng.so"
        ]
      }
    ]
		]
	}],
}
