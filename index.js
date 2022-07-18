let binding;

// Load pre-build binaries for windows and linux 64 bit systems...
if (process.platform === 'win32' && process.arch === 'x64') {
	binding = require('node-gyp-build')(__dirname);
} else if (process.platform === 'linux' && process.arch === 'x64') {
  binding = require('./bin/linux-release/imagy');
} else {
	// ...load self-built binary for other platforms. 32 bit and arm systems are currently unsupported.
	binding = require('./build/Release/imagy');
}

module.exports = binding;
