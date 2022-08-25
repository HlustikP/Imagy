/*
* This is just a scratch/test file for development and can safely be ignored.
*/

#include "pch.h"
#include "decode.h"
#include "encode.h"
#include "imagy.h"

#include "avif.h"

#include <chrono>
#include <boost/gil/extension/io/jpeg.hpp>

namespace gil = boost::gil;

int main(int argc, const char* argv[]) {
  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  auto diff = std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count();

  std::string input = "../../../tests/media/cat.avif";
  std::string output = "cat_out.jpg";

  auto img = imagy::Image(input);
  std::cout << img.GetWidth() << std::endl;
  img.WriteImgToFile(output, imagy::JPEG);

  std::cout << "Done" << std::endl;

  end = std::chrono::system_clock::now();
  diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
  std::cout << "Time elapsed: " << diff << "ms" << std::endl;

	std::cin.get();
	return 0;
}
