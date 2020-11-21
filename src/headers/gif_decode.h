#pragma once
#include <vector>
#include <string>

#include "file_io.h"

namespace utils {

enum BlockIntroducer {
	END_OF_BLOCK	= 0x00,
	EXTENSION		= 0x21,
	IMAGE_SEPERATOR = 0x2c,
	TRAILER			= 0x3b,
};

enum ExtensionLabels {
	PLAIN_TEXT		= 0x01,
	GRAPHIC_CONTROL = 0xf9,
	COMMENT			= 0xfe,
	APPLICATION		= 0xff,
};

struct GifHeaderInfos {
	// See for reference: http://www.matthewflickinger.com/lab/whatsinagif/
	// Signature Header
	char signature[3];			// File type Head ("GIF")
	char version[3];			// GIF version ("87a" or "89a")
	// Logical Screen Descriptor
	uint16_t width;				// Logical screen width
	uint16_t height;			// Logical screen height
	uint8_t packed;				/* Various flags included(most to least significance): 1 Bit global color table boolean, next three Bits ((bits per pixel) - 1),
								   fifth Bit is a boolean for whether the table is sorted by order of importance,
								   last three bits shows the global color table size per color as 2^(X+1), with X beeing the value of the three bits */
	uint8_t bg_index;			// Index of color in color table as default for pixels with undefined color
	uint8_t aspect_ratio;		// If not zero, ratio will be calculated as ((N + 15) / 64) for all N<>0
};

// packed bitflag properties from most to least significant bit (the ints are 3 bit each)
struct Packed {
	bool global_color_table;
	int bit_depth;
	bool sorted;
	int table_size;
};

struct GraphicControl {
	int disposal_method;
	bool user_input;
	bool transparency;
	int transperent_color_index;
	int delay;
};

struct ImageDescriptor {
	uint16_t left_pos;		// left position of where the image should begin drawing
	uint16_t top_pos;		// top position of where the image should begin drawing
	uint16_t witdh;
	uint16_t height;
	uint8_t local_packed;
};

struct LocalPacked {
	bool local_color_table;
	bool interlace;
	bool sorted;
	int local_table_size;	// calculated same way as global color table
};

class DecodeGif {
public:
	DecodeGif(std::string& filename);
	~DecodeGif();
	// DestroyImages(); Implement destructor of images data
	// on demand decoding = new thread for every image?; generator and call next image with next() + option to just load all images at once

	uint16_t ParseBytes(uint8_t least_sig, uint8_t most_sig) const;
	GifHeaderInfos GetInfos() const;
	Packed GetPacked() const;
	GraphicControl GetGraphicControl() const;

  // TODO: Dev only REMOVE LATER!
	const std::vector<uint8_t>* GetImage() const;

	static const int HEADER_SIZE = 13;

private:
	int ValidateHeader(std::string& filename);
	int FillHeaderInfos();
	int InitGlobalColorTable();
	void InitImageVectors();
	int IterateThroughFile();
	uint8_t* HandleExtension(uint8_t* block_data);
	uint8_t* HandleComment(uint8_t* extension_data);
	uint8_t* HandlePlainText(uint8_t* extension_data);
	uint8_t* HandleApplication(uint8_t* extension_data);
	uint8_t* HandleGraphicControl(uint8_t* extension_data);
	uint8_t* HandleImageDescriptor(uint8_t* block_data);
	uint8_t* HandleImageData(uint8_t* block_data);
	int PaintImg(int last_painted_pixel, bool EOI = false);

	GifHeaderInfos infos_;
	Packed packed_;
	GraphicControl graphic_control_;
	ImageDescriptor image_descriptor_;
	LocalPacked local_packed_;

	int image_count_ = 0;
	int current_image_ = 0;
	int file_length_ = 0;
	bool looping_ = false;
	int loop_count_ = 0;
	// gct = global color table
	int gct_size_ = 0;
	std::vector<uint8_t> even_image_;
	std::vector<uint8_t> odd_image_;
	std::vector<uint8_t> temp_image;
	std::vector<uint8_t> global_color_table_;
	uint8_t* file_ = nullptr;
	std::string plain_text_;
	std::string comment_;

	/* LZW stuff: */

	const uint8_t BYTE_SIZE = 8;
	const int MAX_DICTIONARY_CAPACITY = 4095;
	// Flag used to determine whether a dictionary entry has been initialized
	const int INITIALIZATION_FLAG = (1 << 23);
	// Shift to the bit-region where the current code's leading base code is stored
	const int LEADING_CODE_SHIFT = 12;
	// Shift to the bit-region where the current code's appended base code is stored
	const int APPENDED_CODE_SHIFT = 24;
	// Bit mask to access the bit-region where the appended code is stored
	const int APPENDED_CODE_MASK = (0b11111111 << APPENDED_CODE_SHIFT);
	// Note: auto isnt allowed here
	std::vector<uint32_t> dictionary_ = std::vector<uint32_t>(MAX_DICTIONARY_CAPACITY, 0);
	std::vector<uint16_t> code_stream_;
	int base_dic_size_ = 0;
};

} // namespace utils