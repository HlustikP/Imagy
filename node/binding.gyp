{
  'targets': [
    {
      'target_name': 'imagy',
      'cflags!': [ "-fno-exceptions" ],
      'cflags_cc!': [ "-fno-exceptions" ],
      'sources': [ "imagy.cpp" ],
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
	  "conditions": [ ["OS=='win'",
		{
			"configurations": {
				"Release": {
					"msvs_settings": {
						"VCCLCompilerTool": {
							"RuntimeLibrary": 2,
							'ExceptionHandling': 1,
						    "AdditionalOptions": [
                                '-std:c++17',
                                '/EHsc',
                                '/GR'
						    ],
						}
					}
				}
			}
		}
		]
		]
	}
  ],
}
