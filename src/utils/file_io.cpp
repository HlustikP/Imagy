#include "file_io.h"

namespace utils {

FileIO::FileIO(std::string filename) {
	auto* data = GetDataFromFile(filename);
};

/*  Loads binary data from file specified by filename
    Writes size of data buffer into length int reference if provided
    Returns pointer to heap allocated char array containing the data 
	and nullptr on error											*/
char* FileIO::GetDataFromFile(const std::string& filename, int* length, int read_size) {
	std::ifstream input_file;
	input_file.open(filename, std::ios::binary);

	if (!input_file) {
		std::cerr << "Cannot open input file" << std::endl;
		return nullptr;
	}

	// get length of file:
  input_file.seekg(0, input_file.end);
	const auto size = input_file.tellg();

	// tellg() returns -1 if it fails
	if (size == -1) {
		std::cerr << "Cannot tellg() file content length" << std::endl;
		return nullptr;
	}

	// if length pointer is given, write length data into referenced space
	if (length != nullptr) {
		*length = size;
	}

	input_file.seekg(0, std::ifstream::beg);
	auto* buffer = new char[size];

  // read stream as block
  input_file.read(buffer, read_size == 0 ? static_cast<int>(size) : read_size);

	input_file.close();
	return buffer;
};

/*  Creates or overrides file with data, uses binary outstream
    returns 0 on success and 1 on error */
int FileIO::WriteToFile(char* data, std::string filename, int length) {
	std::ofstream output_file(filename, std::ios::binary);

	if (!output_file) {
		std::cerr << "Cannot open input file";
		return 1;
	}

	output_file.write(data, length);
  output_file.close();
	return 0;
};

} // namespace utils
