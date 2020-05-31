#include "image.h"

namespace image
{

Image::Image(std::string& filename) {
	format_ = GetFileExtension(filename);
	if(LoadImgData(filename) == 1) {
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

/*	Load image data into data_ member;
	  returns 0 on success and 1 on error */
int Image::LoadImgData(std::string& filename) {
	switch(format_) {
		case BMP:
			break;
		case JPG:
			break;
		case PNG:
			break;
		case WEBP: {
			auto* file_data = utils::FileIO::GetDataFromFile(filename, &length_);
			data_.reserve(length_);

			std::copy(reinterpret_cast<uint8_t*>(file_data),
				reinterpret_cast<uint8_t*>(file_data) + length_,
				data_.begin());
			delete[] file_data;
			file_data = nullptr;

			if (!WebPGetInfo(&data_[0], length_, &width_, &height_)) {
				return 1;
			}

			break;
		}
		default:
			return 1;
	}

	return 0;
};

/*  TODO: Check format and convert according to it
    Writes data_ into a file specified with filename
    returns 0 on success and 1 on error */
int Image::WriteImgToFile(std::string& filename, ImgFormat format) {
  // no conversion needed
	if (format == format_) {
		return utils::FileIO::WriteToFile(reinterpret_cast<char*>(&data_[0]),
			filename,
			length_);
	}

	return 0;
};

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