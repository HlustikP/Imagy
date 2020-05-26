#pragma once
#include <vector>

#include "decode.h"
#include "gil.hpp"
#include "gil/extension/io/png.hpp"
#include "gil/extension/io/jpeg.hpp"
#include "gil/extension/io/bmp.hpp"

#include "file_io.h"

namespace gil = boost::gil;

// TODO:Create own namespace for this and the utils lib

const enum colorModel
{
	RGB,
	BGR,
	RGBA,
	BGRA
};

class Image
{
public:
	Image(std::string filename);
	Image(std::vector<uint8_t> data, colorModel);

	// maybe destructor for filestream etc closure here?
private:
	
};