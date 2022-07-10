let module;

// Load precompiled binary for windows.
if (process.platform === "win32" && process.arch === "x64") {
	module = require('./bin/winx64/imagy');
} else if(process.platform === "win32" && process.arch === "ia32") {
	//module = require('./bin/winx86/imagy');   CURENTLY UNSUPPORTED
} else {
	// Load self-built binary for other platforms.
	module = require('./build/Release/imagy');
}

module.exports = module;
