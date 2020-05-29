#include "file_io.h"

namespace utils {

FileIO::FileIO(std::string filename) {
	auto* data = GetDataFromFile(filename);
};

/*  Loads binary data from file specified by filename
    Writes size of data buffer into length int reference if provided
    Returns pointer to dynamic char array containing the data */
char* FileIO::GetDataFromFile(std::string filename, int* length) {
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


	input_file.seekg(0, input_file.beg);
	auto* buffer = new char[size];
	std::cout << "Reading " << size << " characters... ";

	// read data as a block:
	input_file.read(buffer, size);

	if (input_file) {
		std::cout << "all characters read successfully.";
	}
	else {
		std::cerr << "error: only " << input_file.gcount() << " could be read";
	}

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
	return 0;
};

} // namespace utils