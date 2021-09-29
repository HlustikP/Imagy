#include "image.h"

namespace image
{

Image::Image(std::string& filename) {
	ImgFormat format = GetFileExtension(filename);

	if(LoadImgData(filename, format) == 1) {
		err_ = "Error while loading image data";
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
  // TODO: find alternative more elegant check method
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

  if (path.extension() == ".gif") {
    return GIF;
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
    case GIF: {
      // Also we will assume animation when the gif file type is used
      animated_ = true;
      gif_ = std::make_unique<utils::DecodeGif>(filename);

      width_ = gif_->GetInfos().width;
      height_ = gif_->GetInfos().height;
      size_ = gif_->GetInfos().height * gif_->GetInfos().width * 4;
      break;
    }
		default:
			return 1;
	}

	return 0;
};

/*  Encodes and writes data_ into a file specified with filename
    returns 0 on success, 1 on encoding error and 2 on format error */
int Image::WriteImgToFile(std::string& filename, ImgFormat format) {
	switch (format) {
	case BMP: {
		const auto byte_rowsize = alpha_ ? width_ * 4 : width_ * 3;
		const auto view = gil::interleaved_view(width_, height_, reinterpret_cast<gil::rgb8_pixel_t*>(&data_[0]), byte_rowsize);
		gil::write_view(filename, view, gil::bmp_tag());
		break;
	}
	case JPEG: {
		const auto byte_rowsize = width_ * 3;
		const auto view = gil::interleaved_view(width_, height_, reinterpret_cast<gil::rgb8_pixel_t*>(&data_[0]), byte_rowsize);
		gil::write_view(filename, view, gil::jpeg_tag());
		break;
	}
	case PNG: {
		const auto byte_rowsize = alpha_ ? width_ * 4 : width_ * 3;
		const auto view = gil::interleaved_view(width_, height_, reinterpret_cast<gil::rgb8_pixel_t*>(&data_[0]), byte_rowsize);
		gil::write_view(filename, view, gil::png_tag());
		break;
	}
	case WEBP: {
    if (!animated_) {
      uint8_t* out_data = nullptr;
      auto out_length = 0;

      // Encode() return 1 or more on error, so return if that happens
      if (EncodeWebp(&out_data, out_length)) {
        err_ = "Error while encoding image";
        return 1;
      }

      utils::FileIO::WriteToFile(reinterpret_cast<char*>(out_data), filename, out_length);

      WebPFree(out_data);
    }
    else {
      DecodeGif(filename);
    }
		break;
	}
	default:
		return 2;
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
	size_ = width_ * height_ * channel_count;
	data_.assign(decoded, decoded + size_);

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
	size_ = width_ * height_ * channel_count;

	data_.reserve(size_);

	std::copy(decoded, decoded + size_, data_.begin());

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
	size_ = width_ * height_ * channel_count;

	data_.reserve(size_);

	std::copy(decoded, decoded + size_, data_.begin());

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
	size_ = width_ * height_ * channel_count;

	data_.reserve(size_);

	std::copy(decoded, decoded + size_, data_.begin());

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
	size_ = width_ * height_ * channel_count;

	data_.reserve(size_);

	std::copy(decoded, decoded + size_, data_.begin());

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
	size_ = width_ * height_ * channel_count;

	data_.reserve(size_);

	std::copy(decoded, decoded + size_, data_.begin());

	return 0;
}

/* Encoding method for WebP only 
	writes data into array pointed to by out_data
	and the data length into the out_length out parameter
	returns 0 on success and 1 on error*/
int Image::EncodeWebp(uint8_t** out_data, int& out_length) {
	out_length = alpha_ ?
		WebPEncodeRGBA(&data_[0], width_, height_, width_ * 4, 100, out_data) :
		WebPEncodeRGB(&data_[0], width_, height_, width_ * 3, 100, out_data);
	
	return out_length ? 0 : 1;
}

int Image::DecodeGif(std::string filename) {
  WebPPicture pic_even;
  WebPPictureInit(&pic_even);
  WebPData webp_data;
  WebPDataInit(&webp_data);
  WebPAnimEncoderOptions enc_options;
  WebPMemoryWriter writer;
  WebPMemoryWriter writer2;
  WebPConfig config;
  config.quality = 90;
  config.sns_strength = 90;
  config.filter_sharpness = 6;
  config.alpha_quality = 90;
  WebPAnimEncoderOptionsInit(&enc_options);
  auto* encoder = WebPAnimEncoderNew(width_, height_, &enc_options);

  pic_even.use_argb = true;
  pic_even.height = height_;
  pic_even.width = width_;
  pic_even.argb_stride = width_;

  WebPMemoryWriterInit(&writer);
  pic_even.writer = WebPMemoryWrite;
  pic_even.custom_ptr = &writer;
  WebPPictureAlloc(&pic_even);

  WebPPicture pic_odd;
  WebPPictureInit(&pic_odd);
  pic_odd.use_argb = true;
  pic_odd.height = height_;
  pic_odd.width = width_;
  pic_odd.argb_stride = width_;

  WebPMemoryWriterInit(&writer2);
  pic_odd.writer = WebPMemoryWrite;
  pic_odd.custom_ptr = &writer2;
  WebPPictureAlloc(&pic_even);

  WebPConfigInit(&config);

  int timestamp = 0;
  bool once = true;

  {
    std::vector<std::future<uint8_t*>> images;
    images.reserve(100);
    std::vector<ImageStatus> gif_statuses;
    gif_statuses.reserve(100);
    std::vector<ImageStatus> webppics_statuses;
    webppics_statuses.reserve(100);
    std::vector<std::future<WebPPicture*>> webppics;
    webppics.reserve(100);
    std::vector<std::future<int>> adders;
    adders.reserve(100);

    std::vector<bool> added_pics;
    added_pics.reserve(100);

    auto racing_gif = 0;
    auto racing_webp = 0;
    auto added_frames = 0;
    gif_statuses.push_back(ImageStatus::UNINITIALIZED);
    webppics_statuses.push_back(ImageStatus::UNINITIALIZED);

    bool gifs_finished = false;
    bool adding_in_progress = false;

    images.push_back(std::async(std::launch::async, [&gif_ = gif_, &gif_statuses, &gifs_finished, racing_gif]() {
      gif_statuses[racing_gif] = ImageStatus::IN_PROGRESS;
      auto result = gif_->Next();
      gif_statuses[racing_gif] = ImageStatus::READY;
      if (result == nullptr) {
        gifs_finished = true;
        gif_statuses[racing_gif] = ImageStatus::NULLIMG;
      }
      return result;
      }));

    while (true) {
      if (gif_statuses[racing_gif] == ImageStatus::IN_USAGE || gif_statuses[racing_gif] == ImageStatus::FINISHED) {

        if (!gifs_finished) {
          racing_gif++;
          gif_statuses.push_back(ImageStatus::UNINITIALIZED);

          images.push_back(std::async(std::launch::async, [&gif_ = gif_, &gif_statuses, &gifs_finished, racing_gif]() {
            gif_statuses[racing_gif] = ImageStatus::IN_PROGRESS;
            auto result = gif_->Next();
            gif_statuses[racing_gif] = ImageStatus::READY;
            if (result == nullptr) {
              gifs_finished = true;
              gif_statuses[racing_gif] = ImageStatus::NULLIMG;
            }
            return result;
            }));
        }
      }

      if (gif_statuses[racing_webp] == ImageStatus::READY && webppics_statuses[racing_webp] == ImageStatus::UNINITIALIZED &&
        (added_frames /*+ 1*/ >= racing_webp)) {
        gif_statuses[racing_webp] = ImageStatus::IN_USAGE;
        webppics_statuses[racing_webp] = ImageStatus::IN_PROGRESS;

        webppics.push_back(std::async(std::launch::async, [&pic_even, &pic_odd, &images, &config, &racing_webp, &webppics_statuses]() {
          auto curr_pic = racing_webp % 2 == 0 ? &pic_even : &pic_odd;
          curr_pic->argb = reinterpret_cast<uint32_t*>(images[racing_webp].get());
          if (!WebPEncode(&config, curr_pic)) {
            std::cout << "Error on frame " << racing_webp << " with data address: " << (int)(curr_pic->argb) << std::endl;
          }
          webppics_statuses[racing_webp] = ImageStatus::FINISHED;
          racing_webp++;
          webppics_statuses.push_back(ImageStatus::UNINITIALIZED);
          return curr_pic;
          }));
      }

      if (webppics_statuses[added_frames] == ImageStatus::FINISHED && !adding_in_progress) {
        timestamp += 50;
        adding_in_progress = true;
        auto pic = webppics[added_frames].get();
        adders.push_back(std::async(std::launch::async, [&encoder, &pic, &config, &adding_in_progress, timestamp]() {
          auto result = WebPAnimEncoderAdd(encoder, pic, timestamp, &config);
          adding_in_progress = false;
          return result;
          }));
        added_frames++;
      }

      if (gifs_finished) {
        if (racing_gif == 0) {
          break;
        }
        else if (webppics_statuses[racing_gif - 1] == ImageStatus::FINISHED) {
          std::cout << "count " << racing_gif << std::endl;
          break;
        }
      }
      /* This gives the OS a hint that it may now consider rescheduling execution of this thread.
         Makes this function more robust against cpu workload fluctuations and generally improves performance */
      std::this_thread::yield();
    }
  }

  WebPAnimEncoderAdd(encoder, NULL, timestamp, NULL);
  WebPAnimEncoderAssemble(encoder, &webp_data);

  utils::FileIO::WriteToFile(const_cast<char*>(reinterpret_cast<const char*>(webp_data.bytes)), filename, webp_data.size);

  WebPDataClear(&webp_data);
  WebPPictureFree(&pic_even);
  WebPPictureFree(&pic_odd);
  WebPMemoryWriterClear(&writer);
  WebPAnimEncoderDelete(encoder);
}

/* Getter for the data length_ property */
int Image::GetLength() const {
	return size_;
}

/* Getter for the height_ property */
int Image::GetHeight() const {
	return height_;
}

/* Getter for the width_ property */
int Image::GetWidth() const {
	return width_;
}

/* Getter for the err_ property 
	returns nullptr if no error */
std::string Image::GetError() const {
	return err_;
}

} // namespace image