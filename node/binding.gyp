{
  'targets': [
    {
      'target_name': 'imagy',
      'cflags!': [ '-fno-exceptions' ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'sources': [
        'src/imagy.cpp',
        'src/async_workers.hpp',
        'src/convert.h',
        'src/convert.cpp',
        'src/image.cpp',
        'src/image.h'
      ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
		    '../src/headers'
     ],
		'link_settings': {
			'libraries': [
			  'image',
			  'utils',
			  'libwebp',
			  'libwebpmux',
			  'libwebpdemux',
			  'jpeg',
			  'libpng16',
			  'zlib'
			],
			'library_dirs': [
        '../src/libs/',
        '../src/libs/Release',
			],
	  },
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
	  'conditions': [ ["OS=='win'",
		{
			'configurations': {
				'Release': {
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
		]
		]
	}],
}
