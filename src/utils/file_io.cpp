#include "file_io.h"

namespace utils {

FileIO::FileIO(std::string filename)
{
	auto data = getDataFromFile(filename);
};

char* FileIO::getDataFromFile(std::string filename, int* length)
{
	std::ifstream inputFile;
	inputFile.open(filename, std::ios::binary);

	if (!inputFile)
	{
		std::cerr << "Cannot open input file" << std::endl;
		return nullptr;
	}

	// get length of file:
	inputFile.seekg(0, inputFile.end);
	auto size = inputFile.tellg();

	// tellg() returns -1 if it fails
	if (size == -1)
	{
		std::cerr << "Cannot tellg() file content length" << std::endl;
		return nullptr;
	}

	// if length pointer is given, write length data into referenced space
	if (length != nullptr)
	{
		*length = size;
	}


	inputFile.seekg(0, inputFile.beg);
	char* buffer = new char[size];
	std::cout << "Reading " << size << " characters... ";

	// read data as a block:
	inputFile.read(buffer, size);

	if (inputFile)
	{
		std::cout << "all characters read successfully.";
	}
	else
	{
		std::cerr << "error: only " << inputFile.gcount() << " could be read";
	}

	// Ab in den Destruktor ?
	inputFile.close();
	return buffer;

	// delete[] buffer;
};

int FileIO::writeToFile(char* data, std::string filename, int length)
{
	std::ofstream outputFile(filename, std::ios::binary);

	if (!outputFile)
	{
		std::cerr << "Cannot open input file";
		return 1;
	}

	outputFile.write(data, length);
	return 0;
};

} // namespace utils