#include "gif_decode.h"

namespace utils {

DecodeGif::DecodeGif(std::string& filename) {
	file_ = reinterpret_cast<uint8_t*>(FileIO::GetDataFromFile(filename, &file_length_));
	ValidateHeader(filename);
	InitGlobalColorTable();
	IterateThroughFile();
};

DecodeGif::~DecodeGif() {
	delete[] file_;
};

int DecodeGif::ValidateHeader(std::string& filename) {
	// if _file is still nullptr, return error 1
	if (!file_) {
		return 1;
	} else if (file_length_ < kHeaderSize + 1) {
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
	gct_size_ = pow(2, packed_.table_size + 1) * 3;

	// global color table follows immediately after the header
	global_color_table_.assign(file_ + kHeaderSize, file_ + gct_size_);

	return 0;
}

int DecodeGif::IterateThroughFile() {
	// skip to the first block after the header-gct-section 
	auto* current_block = file_ + kHeaderSize + gct_size_;

	// TODO: max iteration count?
	while (true) {
		switch (*current_block) {
			case EXTENSION:
				// HandleExtension returns nullptr on success and a pointer to the byte right after the extension on success
				if (!(current_block = HandleExtension(current_block))) {
					return 2;
				}
				break;
			// The TRAILER byte signals end of file
			case TRAILER:
				return 0;
			// If we manage to reach this, we have encountered an unknown block introducer
			default:
				return 1;
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
	// The NETSCAPE2.0 application extension is the only known one, so lets assume it is

	// The only interesting byte is the one that holds information about the loop count, with 0 beeing an infinite loop
	auto LOOP_COUNT_OFFSET = 15;
	extension_data += LOOP_COUNT_OFFSET;
	looping_ = true;
	loop_count_ = *extension_data;

	// The sub block holds to more bytes, one unused one and one block trailer
	extension_data += 2;
	if (*extension_data != END_OF_BLOCK) {
		return nullptr;
	}
	return ++extension_data;
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
	// bit 1 - 3 can be ignored, they're reserved for future use
	// bit 4 - 6 represent the disposal method as a 3-bit int
	std::cout << *extension_data << std::endl;
	graphic_control_.disposal_method = ((*extension_data) << 2) >> 5;
	// bit 7 is the user input flag
	graphic_control_.user_input = (*extension_data) & (1 << 6) == (1 << 6) ? true : false;
	// bit 8 is the transparency flag
	graphic_control_.transparency = (*extension_data) & (1 << 7) == (1 << 7) ? true : false;
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

GifHeaderInfos DecodeGif::GetInfos() const {
	return infos_;
}

Packed DecodeGif::GetPacked() const {
	return packed_;
}

GraphicControl DecodeGif::GetGraphicControl() const {
	return graphic_control_;
}

uint16_t DecodeGif::ParseBytes(uint8_t least_sig, uint8_t most_sig) {
	auto byte_size = 8;
	return (most_sig << byte_size) + least_sig;
}

} // namespace utils