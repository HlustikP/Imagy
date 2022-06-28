{
  'targets': [
    {
      'target_name': 'imagy',
      'cflags!': [ "-fno-exceptions" ],
      'cflags_cc!': [ "-fno-exceptions" ],
      'sources': [ "imagy.cpp" ],
      'include_dirs': [
        "<!@(node -p \"require('node-addon-api').include\")",
		'headers'
     ],
		'link_settings': {
			'libraries': [
			  'image',
			  'utils'
			],
			'library_dirs': [
			  'libs',
			],
	  },
      'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ],
	  "conditions": [ ["OS=='win'",
		{
			"configurations": {
				"Release": {
					"msvs_settings": {
						"VCCLCompilerTool": {
							"RuntimeLibrary": 2
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
