#include "image.h"

namespace image
{

Image::Image(std::string& filename) {
	ImgFormat format = GetFileExtension(filename);

	if(LoadImgData(filename, format) == 1) {
		throw "Error while loading Image data";
	}
	
};

Image::Image(std::vector<uint8_t> data, ColorModel) {
	
};
 
/*  Checks file extension of filename file against known ones
    Returns ImgFormat::INVALID if its unknown or cant be read */
ImgFormat Image::GetFileExtension(std::string& filename) {
	const std::filesystem::path path = filename;

	if(!path.has_extension()) {
		return INVALID;
	}

	// spamming if-checks because switch is illegal with string types
  // TODO: find alternative check method
	if (path.extension() == ".bmp" || path.extension() == ".dib") {
		return BMP;
	}

	if (path.extension() == ".jpg" || path.extension() == ".jpeg" || path.extension() == ".jpe"
		|| path.extension() == ".jif" || path.extension() == ".jfif" || path.extension() == ".jfi") {
		return JPEG;
	}

	if (path.extension() == ".png") {
		return PNG;
	}
  
	if (path.extension() == ".webp") {
		return WEBP;
	}

	return INVALID;
};

// TODO: Implement validation methods to filter out corrupt/forged images
/*	Load image data into data_ as interleaved rbg or rgba vector;
	  returns 0 on success and 1 on error */
int Image::LoadImgData(std::string& filename, ImgFormat format) {
	switch(format) {
		case BMP: {
			// TODO: allow more bit depths (for example including greyscale)
			gil::rgb8_image_t rgb_image;
			gil::rgba8_image_t rgba_image;

			const auto info = gil::read_image_info(filename, gil::bmp_tag());
			const auto color_depth = info._info._bits_per_pixel;

			width_ = info._info._width;
			height_ = info._info._height;

			// we asssume that 32 bit depth will include an alpha channel (rgb + a)
			if (color_depth == 24) {
				alpha_ = false;
				DecodeBmp(rgb_image, filename);
			}
			else if (color_depth == 32) {
				alpha_ = true;
				DecodeBmp(rgba_image, filename);
			}
			else {
				// unknown or unsupported color depth
				return 1;
			}

			break;
		}
		case JPEG: {
			gil::rgb8_image_t rgb_image;
			alpha_ = 0;

			const auto info = gil::read_image_info(filename, gil::jpeg_tag());

			width_ = info._info._width;
			height_ = info._info._height;

			DecodeJpeg(rgb_image, filename);
			break;
		}
		case PNG: {
			// TODO: 8 or 16 bit depth???
			gil::rgb8_image_t rgb_image;
			gil::rgba8_image_t rgba_image;

			const auto info = gil::read_image_info(filename, gil::png_tag());
			const auto color_type = info._info._color_type;

			width_ = info._info._width;
			height_ = info._info._height;

			// for png: color type 2 corresponds to rgb, 6 to rgba
			// see: https://en.wikipedia.org/wiki/Portable_Network_Graphics#Pixel_format
			if (color_type == 2) {
				alpha_ = false;
				DecodePng(rgb_image, filename);
			}
			else if (color_type == 6) {
				alpha_ = true;
				DecodePng(rgba_image, filename);
			}
			else {
				// unknown or unsupported color type
				return 1;
			}

			break;
		}
		case WEBP: {
			auto webp_length = 0;
			auto* file_data = utils::FileIO::GetDataFromFile(filename, &webp_length);

			const auto result = DecodeWebP(reinterpret_cast<uint8_t*>(file_data), webp_length);

			delete[] file_data;
			file_data = nullptr;

			return result;
		}
		default:
			return 1;
	}

	return 0;
};

/*  Encodes and writes data_ into a file specified with filename
    returns 0 on success and 1 on error */
int Image::WriteImgToFile(std::string& filename, ImgFormat format) {
	switch (format) {
	case BMP:
		break;
	case JPEG:
		break;
	case PNG:
		break;
	case WEBP: {
		uint8_t* out_data = nullptr;
		auto out_length = 0;

		Encode(format, &out_data, out_length);
		utils::FileIO::WriteToFile(reinterpret_cast<char*>(out_data), filename, out_length);

		WebPFree(out_data);
		break;
	}
	default:
		return 1;
	}
    
	return 0;
};

int Image::DecodeWebP(uint8_t* data, int in_length) {
	WebPDecoderConfig config;
	if (WebPGetFeatures(data, in_length, &config.input) != VP8_STATUS_OK) {
		return 1;
	}

	alpha_ = config.input.has_alpha;
	const auto channel_count = alpha_ ? 4 : 3;

	auto* decoded = alpha_ ?
		WebPDecodeRGBA(data, in_length, &width_, &height_) :
		WebPDecodeRGB(data, in_length, &width_, &height_);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());
	WebPFree(decoded);
	return 0;
}

int Image::DecodePng(gil::rgb8_image_t image, std::string& filename) {
	gil::read_image(filename, image, gil::png_tag());
	
	// no alpha channel, so only rgb 
	const auto channel_count = 3;

	const auto iter = gil::view(image).begin();
	auto& pixels = *iter;
	auto* decoded = &(pixels[0]);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());

	return 0;
}

int Image::DecodePng(gil::rgba8_image_t image, std::string& filename) {
	gil::read_image(filename, image, gil::png_tag());
	
	// alpha channel included, so rgba
	const auto channel_count = 4;

	const auto iter = gil::view(image).begin();
	auto& pixels = *iter;
	auto* decoded = &(pixels[0]);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());

	return 0;
}

int Image::DecodeJpeg(gil::rgb8_image_t image, std::string& filename) {
	gil::read_image(filename, image, gil::jpeg_tag());

	// no alpha channel, so only rgb 
	const auto channel_count = 3;

	const auto iter = gil::view(image).begin();
	auto& pixels = *iter;
	auto* decoded = &(pixels[0]);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());

	return 0;
}

int Image::DecodeBmp(gil::rgb8_image_t image, std::string& filename) {
	gil::read_image(filename, image, gil::bmp_tag());

	// no alpha channel, so only rgb 
	const auto channel_count = 3;

	const auto iter = gil::view(image).begin();
	auto& pixels = *iter;
	auto* decoded = &(pixels[0]);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());

	return 0;
}

int Image::DecodeBmp(gil::rgba8_image_t image, std::string& filename) {
	gil::read_image(filename, image, gil::bmp_tag());

	// alpha channel included, so rgba
	const auto channel_count = 4;

	const auto iter = gil::view(image).begin();
	auto& pixels = *iter;
	auto* decoded = &(pixels[0]);

	// interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
	length_ = width_ * height_ * channel_count;

	data_.reserve(length_);

	std::copy(decoded, decoded + length_, data_.begin());

	return 0;
}

int Image::Encode(ImgFormat out_format, uint8_t** out_data, int& out_length) {
	switch (out_format) {
	case BMP:
		break;
	case JPEG:
		break;
	case PNG:
		break;
	case WEBP: {
		out_length = alpha_ ?
			WebPEncodeRGBA(&data_[0], width_, height_, width_ * 4, 100, out_data) :
			WebPEncodeRGB(&data_[0], width_, height_, width_ * 3, 100, out_data);
		break;
	}
	default:
		return 1;
	}
	return 0;
}

/* Getter for the data length_ property */
int Image::GetLength() const {
	return length_;
}

/* Getter for the height_ property */
int Image::GetHeight() const {
	return height_;
}

/* Getter for the width_ property */
int Image::GetWidth() const {
	return width_;
}

} // namespace image