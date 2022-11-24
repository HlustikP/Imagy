#include "imagy.h"

namespace imagy
{

Image::Image(std::string& filename) {
	ImgFormat format = GetFileExtension(filename);

  if (format == ImgFormat::INVALID) {
    format = ParseHeader(filename);
  }
  std::cout << format << std::endl;
	if(LoadImgData(filename, format) == 1) {
		err_ = "Error while loading image data";
	}	
};

Image::Image(uint8_t&& data, ColorModel model, int width) {
};
 
/*  Checks file extension of filename file against known ones
    Returns ImgFormat::INVALID if its unknown or cant be read */
ImgFormat Image::GetFileExtension(std::string& filename) {
	const std::filesystem::path path = filename;

	if(!path.has_extension()) {
		return INVALID;
	}

  auto const extension = path.extension();

	// spamming if-checks because switch is illegal with string types
	if (extension == ".bmp" || extension == ".dib") {
		return BMP;
	}

	if (extension == ".jpg" || extension == ".jpeg" || extension == ".jpe"
		|| extension == ".jif" || extension == ".jfif" || extension == ".jfi") {
		return JPEG;
	}

	if (extension == ".png") {
		return PNG;
	}
  
	if (extension == ".webp") {
		return WEBP;
	}

  if (extension == ".gif") {
    return GIF;
  }

  if (extension == ".tiff" || extension == ".tif") {
    return TIFF;
  }

  if (extension == ".avif") {
    return AVIF;
  }

	return INVALID;
};

/* Changes Scale of image using bilinear interpolation as default
   If either aurgument has value 0, it will be calculated to preserve ratio
   Returns 0 on success, 1 on error */
int Image::ChangeScale(unsigned int target_height, unsigned int target_width, InterpolationAlgorithms algorithm) {
  // TODO: Handle animated images
  if (target_height == 0 && target_width == 0) {
    return 1;
  }

  if (target_height == 0) {
    const auto rel_diff = static_cast<double>(target_width) / static_cast<double>(width_);
    target_height = height_ * rel_diff;
  }
  if (target_width == 0) {
    const auto rel_diff = static_cast<double>(target_height) / static_cast<double>(width_);
    target_width = height_ * rel_diff;
  }

  const auto byte_rowsize = alpha_ ? width_ * 4 : width_ * 3;
  gil::rgb8_image_t new_img(target_width, target_height);
  const auto src_view = gil::interleaved_view(width_, height_,
    reinterpret_cast<gil::rgb8_pixel_t*>(&data_[0]), byte_rowsize);

  switch (algorithm) {
    case NEAREST_NEIGHBOUR:
      gil::resize_view(src_view,
        gil::view(new_img),
        gil::nearest_neighbor_sampler{}
      );
      break;
    case BILINEAR:
      [[fallthrough]]; // bilinear IS the default
    default:
      gil::resize_view(src_view,
        gil::view(new_img),
        gil::bilinear_sampler{}
      );
  }

  width_ = target_width;
  height_ = target_height;
  const auto channel_count = alpha_ ? 4 : 3;

  size_ = width_ * height_ * channel_count;
  data_.reserve(size_);

  const auto iter = gil::view(new_img).begin();
  auto& pixels = *iter;

  std::copy_n(&(pixels[0]), size_, data_.begin());

  return 0;
}

int Image::FlipD() {
  // TODO: Handle animated images
  auto forward_itr = &(data_[0]);
  auto backward_itr = &(data_[0]) + size_ - 1;

  if (!alpha_) {
    while (forward_itr < backward_itr) {
      std::swap(*forward_itr, *(backward_itr - 2));
      std::swap(*(forward_itr + 1), *(backward_itr - 1));
      std::swap(*(forward_itr + 2), *(backward_itr));

      forward_itr += 3;
      backward_itr -= 3;
    }
  } else {
    while (forward_itr < backward_itr) {
      std::swap(*forward_itr, *(backward_itr - 3));
      std::swap(*(forward_itr + 1), *(backward_itr - 2));
      std::swap(*(forward_itr + 2), *(backward_itr - 1));
      std::swap(*(forward_itr + 3), *(backward_itr));

      forward_itr += 4;
      backward_itr -= 4;
    }
  }

  return 0;
}

int Image::FlipV() {
  // TODO: Handle animated images
  if (width_ < 2) {
    return 0;
  }

  const uint64_t pixel_size = alpha_ ? 4 : 3;
  const uint64_t line_size = pixel_size * width_;
  auto left_itr = &(data_[0]);
  auto right_itr = &(data_[0]) + line_size - 1;

  for (auto line = 0; line < height_; line++) {
    // reset iterators to the line edges of the next line
    left_itr = &(data_[0]) + line * line_size;
    right_itr = &(data_[0]) + line * line_size + line_size - pixel_size;

    while (left_itr < right_itr) {
      std::swap(*left_itr, *right_itr);
      std::swap(*(left_itr + 1), *(right_itr + 1));
      std::swap(*(left_itr + 2), *(right_itr + 2));

      if (alpha_) {
        std::swap(*(left_itr + 3), *(right_itr + 3));
      }

      left_itr += pixel_size;
      right_itr -= pixel_size;
    }
  }

  return 0;
}

int Image::FlipH() {
  // TODO: Handle animated images
  if (height_ < 2) {
    return 0;
  }

  const uint64_t pixel_size = alpha_ ? 4 : 3;
  const uint64_t line_size = pixel_size * width_;
  auto top_itr = &(data_[0]);
  auto bottom_itr = &(data_[0]) + size_ - line_size;

  // bottom in relation to the image position, not memory position
  while (bottom_itr > top_itr) {
    for (auto i = 0; i < width_; top_itr += pixel_size, bottom_itr += pixel_size, i++) {
      std::swap(*top_itr, *bottom_itr);
      std::swap(*(top_itr + 1), *(bottom_itr + 1));
      std::swap(*(top_itr + 2), *(bottom_itr + 2));

      if (alpha_) {
        std::swap(*(top_itr + 3), *(bottom_itr + 3));
      }
    }

    // move bottom_itr to the beginning of the line above
    bottom_itr -= line_size * 2;
  }

  return 0;
}

ImgFormat Image::ParseHeader(std::string& filename) {
  auto const MAX_SIG_LENGTH = 11;

  auto data = utils::FileIO::GetDataFromFile(filename, nullptr, 11);

  for (auto& sig : img_signatures) {
    auto sig_offset = std::get<1>(sig);
    auto file_itr = data + sig_offset;

    for (auto byte : std::get<0>(sig)) {
      if (byte != *file_itr) {
        break;
      }

      // Check if we've reached the last iteration
      if (data + std::get<0>(sig).size() - 1 == file_itr) {
        return std::get<2>(sig);
      }

      file_itr++;
    }
  }
  delete[] data;
  return ImgFormat::INVALID;
}

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
    case TIFF: {
      gil::rgb8_image_t rgb_image;

      const auto info = gil::read_image_info(filename, gil::tiff_tag());
      const auto color_space = info._info._photometric_interpretation;

      // specification source: https://www.awaresystems.be/imaging/tiff/tifftags/photometricinterpretation.html
      switch (color_space) {
        case 2:
          [[fallthrough]]; // Both 2 and 3 define an RGB color space
        case 3:
          alpha_ = 0;
          break;
        // unknown or unsupported
        default:
        return 1;
      }

      width_ = info._info._width;
      height_ = info._info._height;

      DecodeTiff(rgb_image, filename);
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
    case AVIF:
      DecodeAvif(filename);
      break;
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
  case TIFF: {
    const auto byte_rowsize = alpha_ ? width_ * 4 : width_ * 3;
    const auto view = gil::interleaved_view(width_, height_, reinterpret_cast<gil::rgb8_pixel_t*>(&data_[0]), byte_rowsize);
    gil::write_view(filename, view, gil::tiff_tag());
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

int Image::DecodeTiff(gil::rgb8_image_t image, std::string& filename) {
  gil::read_and_convert_image(filename, image, gil::tiff_tag());

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

int Image::DecodeAvif(std::string& filename) {
  avifRGBImage rgb;
  memset(&rgb, 0, sizeof(rgb));

  auto* decoder = avifDecoderCreate();

  avifResult result = avifDecoderSetIOFile(decoder, filename.c_str());
  if (result != AVIF_RESULT_OK) {
    std::cerr << "Cannot open avif file" << std::endl;
    CleanupAvif(&rgb, decoder);

    return 1;
  }

  result = avifDecoderParse(decoder);
  if (result != AVIF_RESULT_OK) {
    std::cerr << "Failed to decode avif image" << std::endl;
    CleanupAvif(&rgb, decoder);

    return 1;
  }

  auto* img = decoder->image;

  width_ = img->width;
  height_ = img->height;
  const auto bit_depth = img->depth;
  alpha_ = decoder->alphaPresent;

  // RGBA vs RGB
  auto const channel_count = alpha_ ? 4 : 3;
  size_ = width_ * height_ * channel_count;

  while (avifDecoderNextImage(decoder) == AVIF_RESULT_OK) {
    avifRGBImageSetDefaults(&rgb, decoder->image);

    rgb.format = alpha_ ? AVIF_RGB_FORMAT_RGBA : AVIF_RGB_FORMAT_RGB;

    avifRGBImageAllocatePixels(&rgb);

    auto format_conversion_result = avifImageYUVToRGB(decoder->image, &rgb);
    if (format_conversion_result != AVIF_RESULT_OK) {
      std::cerr << "Conversion from YUV to RGB for avif image failed";
      CleanupAvif(&rgb, decoder);
      return 1;
    }

    // Avif supports 8, 10 and 12 bit depths
    switch (bit_depth) {
      case 8:
        data_.reserve(size_);
        std::copy(rgb.pixels, rgb.pixels + size_, data_.begin());
        break;
      case 10:
        // libavif uses two bytes per color per pixel for bit depths > 8
        data_.reserve(size_);

        for (auto i = 0; i < size_ * 2; i += 2) {
          ConvertBitdepthTenToEight((uint16_t&)(rgb.pixels[i]), &(data_[i / 2]));
        }
        break;
      case 12:
        // libavif uses two bytes per color per pixel for bit depths > 8
        data_.reserve(size_);

        for (auto i = 0; i < size_ * 2; i += 2) {
          ConvertBitdepthTwelveToEight((uint16_t&)(rgb.pixels[i]), &(data_[i / 2]));
        }
        break;
      default:
        std::cerr << "Invalid or unsupported avif bit depth" << std::endl;
        return 1;
    }

    // For now we just return, multi-image avif images are to be supported later
    return 0;
  }

  return 0;
}

inline void Image::CleanupAvif(avifRGBImage* rgb, avifDecoder* decoder) {
  avifRGBImageFreePixels(rgb);
  avifDecoderDestroy(decoder);
}

/* The choice of constants makes sure that the minimum and maximum 8 bit outputs are
  mapped only to the minimum and maximum value. This means that only 0 will produce
  a target of 0 and only an input of 0b111111111111 (maximum 12-bit value) will
  produce the 8-bit maximum of 0xFF*/
inline void Image::ConvertBitdepthTenToEight(uint16_t& pixel, uint8_t* target) {
  *target = ((pixel * DEPTH_EIGHT_MULTIPLICATOR) + TEN_TO_EIGHT_DIVISOR_CH) / TEN_TO_EIGHT_DIVISOR;
}

/* The choice of constants makes sure that the minimum and maximum 8 bit outputs are
  mapped only to the minimum and maximum input value. This means that only 0 will produce
  a target of 0 and only an input of 0b1111111111 (maximum 10-bit value) will
  produce the 8-bit maximum of 0xFF*/
inline void Image::ConvertBitdepthTwelveToEight(uint16_t& pixel, uint8_t* target) {
  *target = ((pixel * DEPTH_EIGHT_MULTIPLICATOR) + TWELVE_TO_EIGHT_DIVISOR_CH) / TWELVE_TO_EIGHT_DIVISOR;
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
  WebPAnimEncoderOptionsInit(&enc_options);
  WebPMemoryWriter writer;
  WebPMemoryWriter writer2;
  WebPConfig config;
  config.quality = 90;
  config.sns_strength = 90;
  config.filter_sharpness = 6;
  config.alpha_quality = 90;
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
  std::vector<int> frame_length;
  frame_length.reserve(100);

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

    images.push_back(std::async(std::launch::async, [&gif_ = gif_, &gif_statuses, &gifs_finished, racing_gif, &frame_length]() {
      gif_statuses[racing_gif] = ImageStatus::IN_PROGRESS;
      auto result = gif_->Next();
      frame_length.push_back(gif_->GetGraphicControl().delay * 10);

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

          images.push_back(std::async(std::launch::async, [&gif_ = gif_, &gif_statuses, &gifs_finished, racing_gif, &frame_length]() {
            gif_statuses[racing_gif] = ImageStatus::IN_PROGRESS;
            auto result = gif_->Next();
            frame_length.push_back(gif_->GetGraphicControl().delay * 10);

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
            std::cout << "Error on frame " << racing_webp << " with data address: " << reinterpret_cast<uint64_t>(curr_pic->argb) << std::endl;
          }
          webppics_statuses[racing_webp] = ImageStatus::FINISHED;
          racing_webp++;
          webppics_statuses.push_back(ImageStatus::UNINITIALIZED);
          return curr_pic;
          }));
      }

      if (webppics_statuses[added_frames] == ImageStatus::FINISHED && !adding_in_progress) {
        timestamp += added_frames == 0 ? 0 : frame_length.at(added_frames - 1);
        adding_in_progress = true;
        auto pic = webppics[added_frames].get();
        adders.push_back(std::async(std::launch::async, [&encoder, &pic, &config, &adding_in_progress, timestamp]() {
          auto result = WebPAnimEncoderAdd(encoder, pic, timestamp, &config);
          adding_in_progress = false;
          return result;
          }));
        added_frames++;
      }

      // Exit conditions
      if (gifs_finished) {
        if (racing_gif == 0) {
          // Todo: This should be considered an error!
          break;
        }
        else if (webppics_statuses[racing_gif - 1] == ImageStatus::FINISHED && !adding_in_progress) {
          break;
        }
      }
      /* This gives the OS a hint that it may now consider rescheduling execution of this thread.
         Makes this function more robust against cpu workload fluctuations and generally improves performance */
      std::this_thread::yield();
    }
  }

  WebPAnimEncoderAdd(encoder, NULL, timestamp + frame_length.back(), NULL);
  WebPAnimEncoderAssemble(encoder, &webp_data);

  utils::FileIO::WriteToFile(const_cast<char*>(reinterpret_cast<const char*>(webp_data.bytes)), filename, webp_data.size);

  WebPDataClear(&webp_data);
  WebPPictureFree(&pic_even);
  WebPPictureFree(&pic_odd);
  WebPMemoryWriterClear(&writer);
  WebPAnimEncoderDelete(encoder);

  return 0;
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
