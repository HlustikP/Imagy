#include "image.h"

namespace image
{

Image::Image(std::string& filename)
{
	format_ = GetFileExtension(filename);
};

Image::Image(std::vector<uint8_t> data, ColorModel)
{
	
};

ImgFormat Image::GetFileExtension(std::string& filename)
{
	const std::filesystem::path path = filename;
	if(!path.has_extension())
	{
		return INVALID;
	}

	// spamming if-checks because switch is illegal with string types
	if (path.extension() == ".bmp" || path.extension() == "dib")
	{
		return BMP;
	}
	if (path.extension() == ".jpg" || path.extension() == ".jpeg" || path.extension() == "jpe"
		|| path.extension() == "jif" || path.extension() == "jfif" || path.extension() == "jfi")
	{
		return JPG;
	}
	if (path.extension() == ".png")
	{
		return PNG;
	}
	if (path.extension() == ".webp")
	{
		return WEBP;
	}

	return INVALID;
};

/*	Load image data into data_ member;
	return 0 on success and 1 on error */
int Image::LoadImgData(std::string& filename)
{
	switch(format_) {
		case BMP:
			break;
		case JPG:
			break;
		case PNG:
			break;
		case WEBP:
			std::memmove(&data_[0],
				reinterpret_cast<uint8_t*>(utils::FileIO::getDataFromFile("1_webp_ll.webp", &length_)),
				length_);
			break;
		default:
			return 1;
	}

	return 0;
};

} // namespace image