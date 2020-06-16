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
	if (path.extension() == ".bmp" || path.extension() == "dib") {
		return BMP;
	}

	if (path.extension() == ".jpg" || path.extension() == ".jpeg" || path.extension() == "jpe"
		|| path.extension() == "jif" || path.extension() == "jfif" || path.extension() == "jfi") {
		return JPG;
	}

	if (path.extension() == ".png") {
		return PNG;
	}
  
	if (path.extension() == ".webp") {
		return WEBP;
	}

	return INVALID;
};

/*	Load image data into data_ as interleaved rbg or rgba vector;
	  returns 0 on success and 1 on error */
int Image::LoadImgData(std::string& filename, ImgFormat format) {
	switch(format) {
		case BMP:
			break;
		case JPG:
			break;
		case PNG:
			break;
		case WEBP: {
			auto webp_length = 0;
			auto* file_data = utils::FileIO::GetDataFromFile(filename, &webp_length);

			WebPDecoderConfig config;
			WebPGetFeatures(reinterpret_cast<uint8_t*>(file_data), webp_length, &config.input);
			alpha_ = config.input.has_alpha;
			const auto channel_count = alpha_ ? 4 : 3;

			auto* decoded = alpha_ ? 
				WebPDecodeRGBA(reinterpret_cast<uint8_t*>(file_data), webp_length, &width_, &height_) :
				WebPDecodeRGB(reinterpret_cast<uint8_t*>(file_data), webp_length, &width_, &height_);

      // interleaved rgb(a) stores an amount of bytes equal to the amount of channels times the amount of pixels
			length_ = width_ * height_ * channel_count;

			data_.reserve(length_);

			std::copy(decoded,decoded + length_,data_.begin());

			WebPFree(decoded);
			delete[] file_data;
			file_data = nullptr;

			break;
		}
		default:
			return 1;
	}

	return 0;
};

/*  TODO: Check format and convert according to it
    Encodes and writes data_ into a file specified with filename
    returns 0 on success and 1 on error */
int Image::WriteImgToFile(std::string& filename, ImgFormat format) {
	switch (format) {
	case BMP:
		break;
	case JPG:
		break;
	case PNG:
		break;
	case WEBP: {
		uint8_t* out_data = nullptr;

		const auto out_length = alpha_ ?
			WebPEncodeRGBA(&data_[0], width_, height_, width_ * 4, 100, &out_data) :
			WebPEncodeRGB(&data_[0], width_, height_, width_ * 3, 100, &out_data);

		utils::FileIO::WriteToFile(reinterpret_cast<char*>(out_data), filename, out_length);
		WebPFree(out_data);
		break;
	}
	default:
		return 1;
	}

	return 0;
};

int Image::Convert(ImgFormat out_format) {
	switch (out_format) {
	case BMP:
		break;
	case JPG:
		break;
	case PNG:
		break;
	case WEBP:
		break;
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