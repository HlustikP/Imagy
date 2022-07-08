#pragma once
#include <fstream>
#include <iostream>
#include <vector>

namespace utils {

class FileIO {
public:
	FileIO();
	FileIO(std::string filename);
	static char* GetDataFromFile(const std::string& filename, int* length = nullptr);
	static int WriteToFile(char* data, std::string filename, int length);

private:
	char* _data;
	int _length;
};

} // namespace utils