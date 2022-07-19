/*
* This is just a scratch/test file for development and can safely be ignored.
*/

#include "pch.h"
#include "decode.h"
#include "encode.h"

#include <chrono>
#include <boost/gil/extension/io/jpeg.hpp>

namespace gil = boost::gil;

int main(int argc, const char* argv[]) {
  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  auto diff = std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count();

  std::string input = "cat.jpg";
  std::string output = "cat_out.jpg";

  boost::gil::rgb8_image_t img;

  try {
    auto infos = gil::read_image_info(input, gil::jpeg_tag());
    gil::read_image(input, img, gil::jpeg_tag());
    gil::write_view(output, boost::gil::const_view(img), gil::jpeg_tag());
  } catch (std::exception& e) {
    std::cout << "Error:" << e.what() << std::endl;
  }

  try {
    imagy::Image test_img(input);
    test_img.FlipH();
    test_img.WriteImgToFile(output, imagy::ImgFormat::JPEG);
  }
  catch (std::exception& e) {
    std::cout << "Error:" << e.what() << std::endl;
  }

  //imagy::Image img_rgb8_gif_to_webp(input);
  //img_rgb8_gif_to_webp.WriteImgToFile(output, image::JPEG);

  std::cout << "Done" << std::endl;

  end = std::chrono::system_clock::now();
  diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
  std::cout << "Time elapsed: " << diff << "ms" << std::endl;

	std::cin.get();
	return 0;
}
