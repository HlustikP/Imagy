#include "gif_decode.h"

namespace utils {

DecodeGif::DecodeGif(const std::string& filename) {
	file_ = reinterpret_cast<uint8_t*>(FileIO::GetDataFromFile(filename, &file_length_));
	ValidateHeader(filename);
	InitGlobalColorTable();
	InitImageVectors();
	IterateThroughFile();
};

DecodeGif::~DecodeGif() {
	delete[] file_;
};

int DecodeGif::ValidateHeader(const std::string& filename) {
	// if _file is still nullptr, return error 1
	if (!file_) {
		return 1;
	}
  if (file_length_ < HEADER_SIZE + 1) {
		// header size is 13 bytes + 1 byte file terminator, so return if we're below that
		return 2;
	}

	FillHeaderInfos();
	// TODO: Check Signature

	return 0;
}

int DecodeGif::FillHeaderInfos() {
	auto offset = 0;

	// signature (3 bytes)
	for (auto i = 0; i < 3; i++, offset++) {
		infos_.signature[i] = *(file_ + offset);
	}

	// version (3 bytes)
	for (auto i = 0; i < 3; i++, offset++) {
		infos_.version[i] = *(file_ + offset);
	}

	// width (2 bytes)
	infos_.width = ParseBytes(*(file_ + offset), *(file_ + offset + 1));
	offset += 2;

	// height (2 bytes)
	infos_.height = ParseBytes(*(file_ + offset), *(file_ + offset + 1));
	offset += 2;

	// packed bitflag (1 byte)
	infos_.packed = *(file_ + offset);
	offset++;

	// bg_index (1 byte)
	infos_.bg_index = *(file_ + offset);
	offset++;

	// aspect_ratio (1 byte)
	infos_.aspect_ratio = *(file_ + offset);

	const auto bitflag = infos_.packed;
	packed_.global_color_table = (bitflag & 1 << 7) == (1 << 7) ? true : false;
	// all flags are related to a global color table, so return if we have none
	if (!packed_.global_color_table) {
		return 0;
	}
	// the three bits represent an uint3 value, so we shift 4 to the right to convert
	packed_.bit_depth = ((bitflag & 1 << 6) + (bitflag & 1 << 5) + (bitflag & 1 << 4)) >> 4;
	packed_.sorted = (bitflag & 1 << 3) == (1 << 3) ? true : false;
	packed_.table_size = (bitflag & 1 << 2) + (bitflag & 1 << 1) + (bitflag & 1 << 0);

	return 0;
}

int DecodeGif::InitGlobalColorTable() {
	if (!packed_.global_color_table) {
		return 1;
	}

	// table size is 2^(X+1) times the amount of base colors (rgb = 3)
	gct_size_ = static_cast<int>(pow(2, packed_.table_size + 1)) * 3;

	// global color table follows immediately after the header
	global_color_table_.reserve(gct_size_);
	global_color_table_.assign(file_ + HEADER_SIZE, file_ + HEADER_SIZE + gct_size_);

	return 0;
}

void DecodeGif::InitImageVectors() {
	// Pixel count times the number of colors (RGB = 3)
	const auto memory_size = infos_.width * infos_.height * 3;
	even_image_.reserve(memory_size);
	odd_image_.reserve(memory_size);
  buffer_image_.reserve(memory_size);

	code_stream_.reserve(1000);
}

int DecodeGif::IterateThroughFile() {
	// skip to the first block after the header-gct-section 
	auto* current_block = file_ + HEADER_SIZE + gct_size_;
	// TODO: max iteration count?
	while (true) {
		switch (*current_block) {
			case EXTENSION:
				// HandleExtension returns nullptr on success and a pointer to the byte right after the extension on success
				if (!(current_block = HandleExtension(current_block))) {
					return 2;
				}
				break;
			case IMAGE_SEPERATOR:
				current_block = HandleImageDescriptor(current_block);
				current_block = HandleImageData(current_block);
				break;
			// The TRAILER byte signals end of file
			case TRAILER:
				return 0;
			// If we manage to reach this, we have encountered an unknown block introducer
			default:
        std::cout << "Nope";
        current_block++;
				//return 1;
		}
	}
}

uint8_t* DecodeGif::HandleExtension(uint8_t* block_data) {
	switch (*(++block_data)) {
		case PLAIN_TEXT:
			return HandlePlainText(block_data);
			break;
		case GRAPHIC_CONTROL:
			return HandleGraphicControl(block_data);
			break;
		case COMMENT:
			return HandleComment(block_data);
			break;
		case APPLICATION:
			return HandleApplication(block_data);
			break;
		// If we manage to reach this, we have encountered an unknown extension label
		default:
			return nullptr;
	}
}

// Returns Pointer to the position right after the Comment block, returns nullptr on failure
uint8_t* DecodeGif::HandleComment(uint8_t* extension_data) {
	auto size = 0;
	// Increment by one to get to the byte after the extension label
	extension_data++;

	// The byte after the extension label holds the information about the comment length (number of chars)
	size = *extension_data;
	extension_data++;

	comment_.assign(extension_data, extension_data + size);

	// Increment to first byte after the comment chars
	extension_data += size;

	// Comment extensions have a 0x00 byte as terminator, so if we didnt reach that after the comment chars, something went wrong
	if (*extension_data != END_OF_BLOCK) {
		return nullptr;
	}

	// Return Pointer to the position right after the Comment block
	return ++extension_data;
}

// Returns Pointer to the position right after the Plain Text block, returns nullptr on failure
uint8_t* DecodeGif::HandlePlainText(uint8_t* extension_data) {
	auto size = 0;
	// Increment by one to get to the byte after the extension label
	extension_data++;

	// The byte after the extension label holds the information about the first block size (unused, can be skipped)
	size = *extension_data;
	extension_data++;

	// As the first block is unused, skip over it
	extension_data += size;

	// Lets give this a maximum loop count to prevent the thread from entering an infinite loop on broken/forged images
	auto i = 0;
	while(i < 50) {
		if (*extension_data = END_OF_BLOCK) {
			// Return Pointer to the position right after the Plain Text block
			return ++extension_data;
		}
		// The byte after the extension label holds the information about the plain text length of the current sub block (number of chars)
		size = *extension_data;

		extension_data++;

		plain_text_.insert(plain_text_.end(), extension_data, extension_data + size);

		// Increment to first byte after the Plain Text chars
		extension_data += size;
		i++;
	}

	// If we reach this, something went wrong
	return nullptr;
}

// Returns Pointer to the position right after the Application block, returns nullptr on failure
uint8_t* DecodeGif::HandleApplication(uint8_t* extension_data) {
  extension_data++;

  // Next byte is the next block's size, the specification defines that here this always holds the value 0x0b
  const auto block_size = *extension_data;

  // Load App Identifer and Auth Code
  std::vector<char> ApplicationIdAndAuth;
  ApplicationIdAndAuth.reserve(11);
  for (auto i = 0; i < block_size; i++) {
    ApplicationIdAndAuth.push_back(*(++extension_data));
  }
  
  if (ApplicationIdAndAuth == XmpAppIdAndAuth) {
    return HandleXmpApplication(++extension_data);
  }
  else if (ApplicationIdAndAuth == NetscapeAppIdAndAuth) {
    return HandleNetscapeApplication(++extension_data);
  }

  // TODO: UnknownApplicationExtensionHandler():
  return nullptr;

}

// Returns Pointer to the position right after the Application block, returns nullptr on failure
uint8_t* DecodeGif::HandleNetscapeApplication(uint8_t* extension_data) {

	// The only interesting byte is the one that holds information about the loop count, with 0 beeing an infinite loop
	const auto LOOP_COUNT_OFFSET = 2;
	extension_data += LOOP_COUNT_OFFSET;
	looping_ = true;
	loop_count_ = *extension_data;

	// The sub block holds two more bytes, one unused one and one block trailer
	extension_data += 2;
	if (*extension_data != END_OF_BLOCK) {
		return nullptr;
	}
	return ++extension_data;
}

// Returns Pointer to the position right after the Application block, returns nullptr on failure
uint8_t* DecodeGif::HandleXmpApplication(uint8_t* extension_data) {

  // XMP data is utf-8 encoded and does not use a zero byte, the block itself is however terminated by a double 0x00 block
  while (*extension_data != END_OF_BLOCK) {
    xmp_data.push_back(*extension_data);
    extension_data++;
  }

  return extension_data+=2;
}

// Returns Pointer to the position right after the Graphic Control block, returns nullptr on failure
uint8_t* DecodeGif::HandleGraphicControl(uint8_t* extension_data) {
	auto size = 0;
	// Increment by one to get to the byte after the extension label
	extension_data++;

	// The byte after the extension label holds the information about the block length
	size = *extension_data;
	extension_data++;

	// Handle Packed Field
	// bit 1 is the transparency flag
	graphic_control_.transparency = (*extension_data & (1 << 0)) == (1 << 0) ? true : false;
	// bit 2 is the user input flag
	graphic_control_.user_input = (*extension_data & (1 << 1)) == (1 << 1) ? true : false;
	// bit 3 - 5 represent the disposal method as a 3-bit int
	graphic_control_.disposal_method = ((*extension_data) << 3) >> 5;
	// bit 6 - 8 can be ignored, they're reserved for future use
	extension_data++;

	// Handle delay (16-bit int)
	graphic_control_.delay = ParseBytes(*extension_data, *(extension_data + 1));
	extension_data += 2;

	// Handle transparent color
	graphic_control_.transperent_color_index = *extension_data;
	extension_data++;

	if (*extension_data != END_OF_BLOCK) {
		return nullptr;
	}
	return ++extension_data;
}

uint8_t* DecodeGif::HandleImageDescriptor(uint8_t* block_data) {
	// Increment to first byte after the Image Seperator
	block_data++;

	// Handle left position of where the image should begin drawing (16-bit int)
	image_descriptor_.left_pos = ParseBytes(*block_data, *(block_data + 1));
	block_data += 2;

	// Handle top position of where the image should begin drawing (16-bit int)
	image_descriptor_.top_pos = ParseBytes(*block_data, *(block_data + 1));
	block_data += 2;

	// Handle image width (16-bit int)
	image_descriptor_.witdh = ParseBytes(*block_data, *(block_data + 1));
	block_data += 2;

	// Handle image height (16-bit int)
	image_descriptor_.height = ParseBytes(*block_data, *(block_data + 1));
	block_data += 2;

	// Handle local Packed Field
	local_packed_.local_color_table = (*block_data & (1 << 0)) == (1 << 0) ? true : false;
	local_packed_.interlace = (*block_data & (1 << 1)) == (1 << 1) ? true : false;
	local_packed_.sorted = (*block_data & (1 << 2)) == (1 << 2) ? true : false;
	block_data++;

	// There is no block terminator for the Image Descriptor as it is always followed immediately by image data (or local color table if used)
	return block_data;
};

uint8_t* DecodeGif::HandleImageData(uint8_t* block_data) {
	// TODO: Document this mess

	auto next_pixel = 0;

	// 0-Re-Initialize dictionary
	std::fill(dictionary_.begin(), dictionary_.end(), 0);

	// The minimum code size is stored inside the first byte of the image data
	const auto MIN_CODE_SIZE = *block_data;
	block_data++;

	// The Dictionary's initial size is equal to 2^minimum code size, + 2 for the two special codes (CLEAR and EOI)
	auto dic_size = (1 << MIN_CODE_SIZE) + 2;
	base_dic_size_ = dic_size;

	const auto CLEAR = dic_size - 2;
	const auto EOI = dic_size - 1;

	// Initialize our base dictionary
	for (auto i = 0; i < dic_size; i++) {
		dictionary_[i] = (i << LEADING_CODE_SHIFT) | INITIALIZATION_FLAG | (i << APPENDED_CODE_SHIFT);
	}

	// The first sub-block's size is stored inside the second byte of the image data
	auto sub_block_size = *block_data;

	// Increment to third byte (First byte containing actual image code data)
	block_data++;

	// Increment code size by one, because we filled up the dictionary to the full capacity of the previous code
	auto curr_code_size = MIN_CODE_SIZE + 1;

	auto curr_bit = 0;
  auto curr_code = 0;
  auto last_code = 0;
	const auto shift = BYTE_SIZE - curr_code_size;

	// Access second stored code as the first code is always CLEAR
  block_data = ParseOneRound(curr_code_size, curr_bit, last_code, sub_block_size, block_data);
  block_data = ParseOneRound(curr_code_size, curr_bit, last_code, sub_block_size, block_data);
	//auto last_code = static_cast<uint8_t>(*block_data << (BYTE_SIZE - (curr_code_size * 2))) >> shift;

	code_stream_.push_back(last_code);
	auto buffered_code = dictionary_.at(last_code);
  auto test = 0;
	//curr_bit = curr_code_size * 2;
	curr_code = 0;

	while (true) {
		// Increase code size by one bit if current size reaches maximum capacity
		if (dic_size == static_cast<int>(pow(2, curr_code_size))) {
      if (curr_code_size != MAX_CODE_SIZE) {
        curr_code_size++;
      }
		}

		// Determine how many bits left and right need to be kicked out via shift to only analyze the current code
		auto lshift = BYTE_SIZE - curr_code_size - curr_bit;
		auto rshift = BYTE_SIZE - curr_code_size;
		if (lshift < 0) {
			lshift = 0;
			rshift = curr_bit;
		}
		curr_code = static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift);
		curr_bit += curr_code_size;
		// Code handling routine for if the current code doesnt reach the end of the current byte
		if (curr_bit <= BYTE_SIZE) {
			if (curr_code == EOI) {
				PaintImg(next_pixel, true);
        code_stream_.clear();
				break;
			}
			// Reset dictionary to it's base state and set code_size back to the initial value
			if (curr_code == CLEAR) {
        next_pixel = PaintImg(next_pixel);
				std::fill(dictionary_.begin() + base_dic_size_, dictionary_.end(), 0);
				curr_code_size = MIN_CODE_SIZE + 1;
        dic_size = (1 << MIN_CODE_SIZE) + 2;
        block_data = ParseOneRound(curr_code_size, curr_bit, last_code, sub_block_size, block_data);
        code_stream_.clear();
        code_stream_.push_back(last_code);
        buffered_code = dictionary_.at(last_code);

        continue;
			}

      // Special subroutine to wrap up handling bit-parsing for when the current code ends exactly at the end of the current byte
      if (curr_bit == BYTE_SIZE) {
        block_data++;
        sub_block_size--;
        if (sub_block_size == 0) {
          sub_block_size = *block_data;
          if (sub_block_size == 0) {
            // TODO: Throw warning that the next data sub block is of size 0 but EOI hasnt been reached yet
            return ++block_data;
          }
          block_data++;
        }
        curr_bit = 0;
      }

			// Check if Dictionary entry has been initialized
			if ((dictionary_[curr_code] & INITIALIZATION_FLAG) == INITIALIZATION_FLAG) {
				buffered_code = dictionary_[curr_code];
			}
		}
		// Code handling routine for if the current code reaches beyond the end of the current byte
		else {
			block_data++;
			sub_block_size--;
			if (sub_block_size == 0) {
				sub_block_size = *block_data;
				if (sub_block_size == 0) {
					// TODO: Throw warning that the next data sub block is of size 0 but EOI hasnt been reached yet
					return ++block_data;
				}
				block_data++;
			}

			const auto remaining_bits = curr_bit - BYTE_SIZE;
			curr_bit = 0;

			lshift = BYTE_SIZE - remaining_bits - curr_bit;
			rshift = BYTE_SIZE - remaining_bits;
			if (lshift < 0) {
				lshift = 0;
				rshift = curr_bit;
			}

			// Having to chain-cast to uint8_t here because otherwise the bits wont be shifted "off the edge"
			curr_code += static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift) << (curr_code_size - remaining_bits);

			curr_bit += remaining_bits;

      if (curr_bit > BYTE_SIZE) {
        block_data++;
        sub_block_size--;
        if (sub_block_size == 0) {
          sub_block_size = *block_data;
          if (sub_block_size == 0) {
            // TODO: Throw warning that the next data sub block is of size 0 but EOI hasnt been reached yet
            return ++block_data;
          }
          block_data++;
        }

        const auto remaining_bits = curr_bit - BYTE_SIZE;
        curr_bit = 0;

        lshift = BYTE_SIZE - remaining_bits - curr_bit;
        rshift = BYTE_SIZE - remaining_bits;
        if (lshift < 0) {
          lshift = 0;
          rshift = curr_bit;
        }

        // Having to chain-cast to uint8_t here because otherwise the bits wont be shifted "off the edge"
        curr_code += static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift) << (curr_code_size - remaining_bits);

        curr_bit += remaining_bits;
      }

			if (curr_code == EOI) {
				PaintImg(next_pixel, true);
        code_stream_.clear();
				break;
			}

      if (curr_bit == BYTE_SIZE) {
        block_data++;
        sub_block_size--;
        if (sub_block_size == 0) {
          sub_block_size = *block_data;
          if (sub_block_size == 0) {
            // TODO: Throw warning that the next data sub block is of size 0 but EOI hasnt been reached yet
            return ++block_data;
          }
          block_data++;
        }

        curr_bit = 0;
      }

			// Reset dictionary to it's base state and set code_size back to the initial value
			if (curr_code == CLEAR) {
				next_pixel = PaintImg(next_pixel);
				std::fill(dictionary_.begin() + base_dic_size_, dictionary_.end(), 0);
				curr_code_size = MIN_CODE_SIZE + 1;
        dic_size = (1 << MIN_CODE_SIZE) + 2;
        block_data = ParseOneRound(curr_code_size, curr_bit, last_code, sub_block_size, block_data);
        code_stream_.clear();
        code_stream_.push_back(last_code);
        buffered_code = dictionary_.at(last_code);

        continue;
			}
			// Check if Dictionary entry has been initialized
			if ((dictionary_[curr_code] & INITIALIZATION_FLAG) == INITIALIZATION_FLAG) {
				buffered_code = dictionary_[curr_code];
			}
		}
		// LZW Decoding Round
		code_stream_.push_back(curr_code);
		const auto last_code_first_symbol = (dictionary_[last_code] & LEADING_CODE_MASK);
		dictionary_[dic_size] = last_code | last_code_first_symbol | INITIALIZATION_FLAG |
			((buffered_code & LEADING_CODE_MASK) << LEADING_CODE_SHIFT);
		dic_size++;
		last_code = curr_code;
	}

	current_image_++;
	image_count_++;
	return block_data + 2;
}

int DecodeGif::PaintImg(int next_pixel_to_paint, bool EOI) {
	// As we paint the Image in reverse, jump to last pixel's index of the current segment
	auto curr_pixel = (image_descriptor_.height * image_descriptor_.witdh - 1) * 3;
  const auto tail_pixel = curr_pixel;

	// Choose our image vector from 2 alternating ones
  auto* const curr_image = (current_image_ % 2) == 0 ? &even_image_ : &odd_image_;
  auto* const last_image = (current_image_ % 2) == 0 ? &odd_image_ : &even_image_;

  // Initialize image base during first painting iteration
  if (next_pixel_to_paint == 0) {
	  switch (graphic_control_.disposal_method) {

		  // Method 0: Do nothing
	  case 0:
		  break;

		  // Method 1: Use the last image as a background to be overwritten
	  case 1:
		  //*curr_image = *last_image; <- size is 0, so this doesnt work
      std::copy(&(*last_image)[0], &(*last_image)[0] + last_image->capacity(), &(*curr_image)[0]);
		  break;

		  // Method 2: Reset Image to the background color
	  case 2: {
		  // Pixels are here broken down into their color components, therefore the "* 3"
		  const auto pixel_count = infos_.width * infos_.height * 3;
		  const auto background_color = infos_.bg_index;
		  const auto red = global_color_table_[background_color * 3];
		  const auto green = global_color_table_[(background_color * 3) + 1];
		  const auto blue = global_color_table_[(background_color * 3) + 2];
		  for (auto i = 0; i < pixel_count - 1; i = i + 3) {
			  (*curr_image)[i] = red;
			  (*curr_image)[i + 1] = green;
			  (*curr_image)[i + 2] = blue;
		  }
	  }
		break;
		// If the default is reached, then something's not quite right
	  default:
		  break;
	  }
  }

	// Calculate starting position, see https://i.imgur.com/7mc5yar.png for an attempt at an explanation
	auto bottom_right_pixel = (((image_descriptor_.top_pos * infos_.width) + image_descriptor_.left_pos) +
					((image_descriptor_.height - 1) * infos_.width) + image_descriptor_.witdh - 1) * 3;
    
	// Note to future self: if the to-be-drawn rectangle isnt as wide as the img, we have to skip all pixels that hang over left/right off the sides!
	if (next_pixel_to_paint == 0 && EOI) {
    // Images encoded entirely within one round can be directly written into the actual image vector...
    (infos_.height == image_descriptor_.height && infos_.width == image_descriptor_.witdh) ?
      // Use simplified routine if the entire image is redrawn
      DecodeLZWToRGB(curr_image, curr_pixel) :
      // Use more complex routine if just a section is redrawn
      DecodeLZWToRGB(curr_image, bottom_right_pixel, infos_.width, image_descriptor_.left_pos, image_descriptor_.top_pos);
	}
  else {
    // ... otherwise we first write the data into a buffer image and then copy them over
    auto const head_pixel = (infos_.height == image_descriptor_.height && infos_.width == image_descriptor_.witdh) ?
      // Use simplified routine if the entire image is redrawn
      DecodeLZWToRGB(&buffer_image_, curr_pixel) :
      // Use more complex routine if just a section is redrawn
      DecodeLZWToRGB(&buffer_image_, bottom_right_pixel, infos_.width, image_descriptor_.left_pos, image_descriptor_.top_pos);
    auto const buffer_size = tail_pixel - head_pixel;
    std::copy(&buffer_image_[head_pixel], &buffer_image_[head_pixel] + buffer_size + 3, &(*curr_image)[next_pixel_to_paint]);
    next_pixel_to_paint += buffer_size;
	}
	return next_pixel_to_paint + 3;
}

GifHeaderInfos DecodeGif::GetInfos() const {
	return infos_;
}

Packed DecodeGif::GetPacked() const {
	return packed_;
}

GraphicControl DecodeGif::GetGraphicControl() const {
	return graphic_control_;
}

const std::vector<uint8_t>* DecodeGif::GetImage() const {
	return &odd_image_;
}

uint16_t DecodeGif::ParseBytes(uint8_t least_sig, uint8_t most_sig) const {
	return (most_sig << BYTE_SIZE) + least_sig;
}

uint8_t* DecodeGif::ParseOneRound(int curr_code_size, int& curr_bit, int& curr_code, uint8_t& sub_block_size, uint8_t* block_data) {
  const auto BYTE_SIZE = 8;

  // Determine how many bits left and right need to be kicked out via shift to only analyze the current code
  auto lshift = BYTE_SIZE - curr_code_size - curr_bit;
  auto rshift = BYTE_SIZE - curr_code_size;
  if (lshift < 0) {
    lshift = 0;
    rshift = curr_bit;
  }
  curr_code = static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift);
  curr_bit += curr_code_size;

  // Code handling routine for if the current code doesnt reach the end of the current byte
  if (curr_bit <= BYTE_SIZE) {
    // Special subroutine to wrap up handling bit-parsing for when the current code ends exactly at the end of the current byte
    if (curr_bit == BYTE_SIZE) {
      block_data++;
      sub_block_size--;
      curr_bit = 0;
    }
  }
  // Code handling routine for if the current code reaches beyond the end of the current byte
  else {
    block_data++;
    sub_block_size--;

    const auto remaining_bits = curr_bit - BYTE_SIZE;
    curr_bit = 0;

    lshift = BYTE_SIZE - remaining_bits - curr_bit;
    rshift = BYTE_SIZE - remaining_bits;
    if (lshift < 0) {
      lshift = 0;
      rshift = curr_bit;
    }

    // Having to chain-cast to uint8_t here because otherwise the bits wont be shifted "off the edge"
    curr_code += static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift) << (curr_code_size - remaining_bits);

    curr_bit += remaining_bits;
  }
  if (curr_bit == BYTE_SIZE) {
    block_data++;
    sub_block_size--;
    curr_bit = 0;
  }
  if (curr_bit > BYTE_SIZE) {
    block_data++;
    sub_block_size--;

    const auto remaining_bits = curr_bit - BYTE_SIZE;
    curr_bit = 0;

    lshift = BYTE_SIZE - remaining_bits - curr_bit;
    rshift = BYTE_SIZE - remaining_bits;
    if (lshift < 0) {
      lshift = 0;
      rshift = curr_bit;
    }

    // Having to chain-cast to uint8_t here because otherwise the bits wont be shifted "off the edge"
    curr_code += static_cast<uint8_t>(static_cast<uint8_t>(*block_data << lshift) >> rshift) << (curr_code_size - remaining_bits);

    curr_bit += remaining_bits;
  }

  return block_data;
}

// Returns relative byte position of the head pixel
int DecodeGif::DecodeLZWToRGB(std::vector<uint8_t>* image, int curr_pixel)
{
  for (signed int code = code_stream_.size() - 1; code >= 0; code--) {
    auto curr_code = code_stream_[code];
    // If we directly read a base code, write corresponding rgb-values into the image and move on to the next code
    if (curr_code < base_dic_size_) {
      (*image)[curr_pixel] = global_color_table_[curr_code * 3];
      (*image)[curr_pixel + 1] = global_color_table_[(curr_code * 3) + 1];
      (*image)[curr_pixel + 2] = global_color_table_[(curr_code * 3) + 2];
      curr_pixel -= 3;
      continue;
    }
    // Get the rgb-values from the appended code and dive deeper until we hit a base code
    while (true) {
      const auto dic_entry = dictionary_[curr_code];
      (*image)[curr_pixel] = global_color_table_[((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3];
      (*image)[curr_pixel + 1] = global_color_table_[(((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3) + 1];
      (*image)[curr_pixel + 2] = global_color_table_[(((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3) + 2];
      curr_pixel -= 3;
      if (curr_code < base_dic_size_) {
        // We hit a base code and therefore exit the inner loop
        break;
      }
      curr_code = dic_entry & NEXT_CODE_MASK;
    }
  }

  // Increment by three to get back to the last pixel written
  return curr_pixel + 3;
}

// Returns relative byte position of the head pixel
int DecodeGif::DecodeLZWToRGB(std::vector<uint8_t>* image, int curr_pixel, int width, int anchor_x, int anchor_y)
{
  const auto horizontal_pad = (width - image_descriptor_.witdh) * 3;
  const auto rightmost_pos = image_descriptor_.witdh - 1;
  auto inner_image_pos = rightmost_pos;

  for (signed int code = code_stream_.size() - 1; code >= 0; code--) {
    auto curr_code = code_stream_[code];
    // If we directly read a base code, write corresponding rgb-values into the image and move on to the next code
    if (curr_code < base_dic_size_) {
      (*image)[curr_pixel] = global_color_table_[curr_code * 3];
      (*image)[curr_pixel + 1] = global_color_table_[(curr_code * 3) + 1];
      (*image)[curr_pixel + 2] = global_color_table_[(curr_code * 3) + 2];
      curr_pixel -= 3;
      if (inner_image_pos == 0) {
        curr_pixel -= horizontal_pad;
        inner_image_pos = rightmost_pos;
      }
      else {
        inner_image_pos--;
      }
      continue;
    }
    // Get the rgb-values from the appended code and dive deeper until we hit a base code
    while (true) {
      const auto dic_entry = dictionary_[curr_code];
      (*image)[curr_pixel] = global_color_table_[((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3];
      (*image)[curr_pixel + 1] = global_color_table_[(((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3) + 1];
      (*image)[curr_pixel + 2] = global_color_table_[(((dic_entry & APPENDED_CODE_MASK) >> APPENDED_CODE_SHIFT) * 3) + 2];
      curr_pixel -= 3;
      if (inner_image_pos == 0) {
        curr_pixel -= horizontal_pad;
        inner_image_pos = rightmost_pos;
      }
      else {
        inner_image_pos--;
      }
      if (curr_code < base_dic_size_) {
        // We hit a base code and therefore exit the inner loop
        break;
      }
      curr_code = dic_entry & NEXT_CODE_MASK;
    }
  }

  // Increment by three to get back to the last pixel written
  return curr_pixel + 3;
}

} // namespace utils