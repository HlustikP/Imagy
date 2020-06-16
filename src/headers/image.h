#pragma once
#include <vector>
#include <regex>
#include <filesystem>

#include "decode.h"
#include "encode.h"
#include "gil.hpp"
#include "gil/extension/io/png.hpp"
#include "gil/extension/io/jpeg.hpp"
#include "gil/extension/io/bmp.hpp"

#include "file_io.h"

namespace gil = boost::gil;

namespace image
{

const enum ColorModel {
	RGB,
	BGR,
	RGBA,
	BGRA
};

const enum ImgFormat {
	BMP,
	JPG,
	PNG,
	WEBP,

	// always have this as last entry
	INVALID = 99,
};

class Image {
public:
  Image(std::string& filename);
  Image(std::vector<uint8_t> data, ColorModel); // NOT YET IMPLEMENTED

  int WriteImgToFile(std::string& filename, ImgFormat format);

  int GetLength() const;
  int GetHeight() const;
  int GetWidth() const;

  // maybe destructor for filestream etc closure here?
private:
  static ImgFormat GetFileExtension(std::string& filename);
  int LoadImgData(std::string& filename, ImgFormat format);

  int Decode(ImgFormat in_format, uint8_t* data, int in_length);
  int Encode(ImgFormat out_format, uint8_t** out_data, int& out_length);

  // rgb(a) data
  std::vector<uint8_t> data_;
  bool alpha_;
  int length_ = 0;
  int height_ = 0;
  int width_ = 0;
};

} // namespace image