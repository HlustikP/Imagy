#pragma once
#include <fstream>
#include <iostream>

namespace utils {

class FileIO {
public:
	FileIO();
	FileIO(std::string filename);
	static char* GetDataFromFile(std::string& filename, int* length = nullptr);
	static int WriteToFile(char* data, std::string filename, int length);

private:
	char* data;
	int length;
};

} // namespace utils