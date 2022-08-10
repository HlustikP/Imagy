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
		    'src/headers',
			'bin/x64-release/_deps/boost-src',
			'bin/linux-release/_deps/boost-src'
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
			    'jpeg-static',
			    'libpng16',
          'tiff',
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
          "../src/libs/linux-release/libutils.a",
          "../src/libs/linux-release/libimage.a",
          "/opt/libjpeg-turbo/lib64/libjpeg.a",
          "/usr/local/lib/libwebpmux.a",
          "/usr/local/lib/libwebpdemux.a",
          "/usr/local/lib/libwebp.a",
          "/usr/local/lib/libpng.a",
          "/usr/local/lib/libtiff.a",
          "/usr/local/lib/libz.a"
        ]
      }
    ]
		]
	}],
}
