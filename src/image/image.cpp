#include "image.h"

namespace image
{

Image::Image(std::string& filename) {
	format_ = GetFileExtension(filename);
	if(LoadImgData(filename) == 1)
	{
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
			std::memmove(&data_[0],
				reinterpret_cast<uint8_t*>(file_data),
				length_);
			delete[] file_data;
			file_data = nullptr;
			break;
		}
		default:
			return 1;
	}

	return 0;
};

} // namespace image