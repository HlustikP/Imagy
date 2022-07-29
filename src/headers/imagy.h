#pragma once
#include <vector>
#include <filesystem>
#include <thread>
#include <future>

// webp
#include "decode.h"
#include "encode.h"
#include "mux.h"
#include "demux.h"
// boost::gil
#include "boost/gil.hpp"
#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>

#include "file_io.h"
#include "gif_decode.h"

namespace gil = boost::gil;

namespace imagy
{

enum ColorModel {
	RGB,
	BGR,
	RGBA,
	BGRA,
};

enum ImgFormat {
	BMP,
	JPEG,
	PNG,
	WEBP,
	GIF,

	// always have this as last entry
	INVALID = 99,
};

enum ImageStatus {
  UNINITIALIZED,
  IN_PROGRESS,
  READY,
  IN_USAGE,
  FINISHED,
  NULLIMG
};

enum InterpolationAlgorithms {
  NEAREST_NEIGHBOUR,
  BILINEAR
};

const static std::vector<uint8_t> GIF_SIG = { 0x47, 0x49, 0x46 };
const static std::vector<uint8_t> JPEG_SIG = { 0xFF, 0xD8 };
const static std::vector<uint8_t> PNG_SIG = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
const static std::vector<uint8_t> BMP_SIG = { 0x42, 0x4D };
const static std::vector<uint8_t> WEBP_SIG = { 0x57, 0x45, 0x42, 0x50 };

// Tuple types: File header/signature, Offset of signature begin, corresponding file type
static inline std::vector<std::tuple<const std::vector<uint8_t>, int, ImgFormat>> img_signatures {
  std::tuple(GIF_SIG, 0, GIF),
	std::tuple(JPEG_SIG, 0, JPEG),
	std::tuple(PNG_SIG, 0, PNG),
	std::tuple(BMP_SIG, 0, BMP),
	std::tuple(WEBP_SIG, 8, WEBP)
};

class Image {
public:
  Image(std::string& filename);
  Image(uint8_t&& data, ColorModel model, int width); // NOT YET IMPLEMENTED

  int WriteImgToFile(std::string& filename, ImgFormat format);
	static ImgFormat GetFileExtension(std::string& filename);

  int ChangeScale(unsigned int target_height, unsigned int target_width, InterpolationAlgorithms algorithm = BILINEAR);
  int FlipD();
  int FlipV();
  int FlipH();

  static ImgFormat ParseHeader(std::string& filename);

  int GetLength() const;
  int GetHeight() const;
  int GetWidth() const;
  std::string GetError() const;

private:
  int LoadImgData(std::string& filename, ImgFormat format);

  int DecodeWebP(uint8_t* data, int in_length);
  int DecodePng(gil::rgb8_image_t image, std::string& filename);
  int DecodePng(gil::rgba8_image_t image, std::string& filename);
  int DecodeJpeg(gil::rgb8_image_t image, std::string& filename);
  int DecodeBmp(gil::rgb8_image_t image, std::string& filename);
  int DecodeBmp(gil::rgba8_image_t image, std::string& filename);

  int EncodeWebp(uint8_t** out_data, int& out_length);
  int DecodeGif(std::string filename);

  std::string err_ = "";

  // rgb(a) data
  std::vector<uint8_t> data_;

  std::unique_ptr<utils::DecodeGif> gif_;

  bool alpha_;
  bool animated_ = false;
  int size_ = 0;
  int height_ = 0;
  int width_ = 0;
};

} // namespace image
