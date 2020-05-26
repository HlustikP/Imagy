#pragma once
#include <fstream>
#include <iostream>

namespace utils {

class FileIO {
public:
	FileIO();
	FileIO(std::string filename);
	static char* getDataFromFile(std::string filename, int* length = nullptr);
	static int writeToFile(char* data, std::string filename, int length);

private:
	char* data;
	int length;
};

} // namespace utils