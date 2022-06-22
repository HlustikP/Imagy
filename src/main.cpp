#include "pch.h"
#include "decode.h"
#include "encode.h"

#include <chrono>

namespace gil = boost::gil;

void test() {
	std::vector<std::pair<std::string, std::string>> test_filenames;
	test_filenames.push_back(std::pair("test\\rgb8.webp", "test\\results\\rgb8_webp_to.webp"));
	test_filenames.push_back(std::pair("test\\rgb8.png", "test\\results\\rgb8_png_to.webp"));
	test_filenames.push_back(std::pair("test\\rgb8.jpg", "test\\results\\rgb8_jpg_to.webp"));
	test_filenames.push_back(std::pair("test\\rgb8.bmp", "test\\results\\rgb8_bmp_to.webp"));

	test_filenames.push_back(std::pair("test\\rgb8.webp", "test\\results\\rgb8_webp_to.bmp"));
	test_filenames.push_back(std::pair("test\\rgb8.png", "test\\results\\rgb8_png_to.bmp"));
	test_filenames.push_back(std::pair("test\\rgb8.jpg", "test\\results\\rgb8_jpg_to.bmp"));
	test_filenames.push_back(std::pair("test\\rgb8.bmp", "test\\results\\rgb8_bmp_to.bmp"));

	test_filenames.push_back(std::pair("test\\rgb8.webp", "test\\results\\rgb8_webp_to.jpg"));
	test_filenames.push_back(std::pair("test\\rgb8.png", "test\\results\\rgb8_png_to.jpg"));
	test_filenames.push_back(std::pair("test\\rgb8.jpg", "test\\results\\rgb8_jpg_to.jpg"));
	test_filenames.push_back(std::pair("test\\rgb8.bmp", "test\\results\\rgb8_bmp_to.jpg"));

	test_filenames.push_back(std::pair("test\\rgb8.webp", "test\\results\\rgb8_webp_to.png"));
	test_filenames.push_back(std::pair("test\\rgb8.png", "test\\results\\rgb8_png_to.png"));
	test_filenames.push_back(std::pair("test\\rgb8.jpg", "test\\results\\rgb8_jpg_to.png"));
	test_filenames.push_back(std::pair("test\\rgb8.bmp", "test\\results\\rgb8_bmp_to.png"));

	auto time_start = std::chrono::high_resolution_clock::now();

	image::Image img_rgb8_webp_to_webp(test_filenames[0].first);
	img_rgb8_webp_to_webp.WriteImgToFile(test_filenames[0].second, image::WEBP);
	image::Image img_rgb8_png_to_webp(test_filenames[1].first);
	img_rgb8_png_to_webp.WriteImgToFile(test_filenames[1].second, image::WEBP);
	image::Image img_rgb8_jpg_to_webp(test_filenames[2].first);
	img_rgb8_jpg_to_webp.WriteImgToFile(test_filenames[2].second, image::WEBP);
	image::Image img_rgb8_bmp_to_webp(test_filenames[3].first);
	img_rgb8_bmp_to_webp.WriteImgToFile(test_filenames[3].second, image::WEBP);

	image::Image img_rgb8_webp_to_bmp(test_filenames[4].first);
	img_rgb8_webp_to_bmp.WriteImgToFile(test_filenames[4].second, image::BMP);
	image::Image img_rgb8_png_to_bmp(test_filenames[5].first);
	img_rgb8_png_to_bmp.WriteImgToFile(test_filenames[5].second, image::BMP);
	image::Image img_rgb8_jpg_to_bmp(test_filenames[6].first);
	img_rgb8_jpg_to_bmp.WriteImgToFile(test_filenames[6].second, image::BMP);
	image::Image img_rgb8_bmp_to_bmp(test_filenames[7].first);
	img_rgb8_bmp_to_bmp.WriteImgToFile(test_filenames[7].second, image::BMP);

	image::Image img_rgb8_webp_to_jpg(test_filenames[8].first);
	img_rgb8_webp_to_jpg.WriteImgToFile(test_filenames[8].second, image::JPEG);
	image::Image img_rgb8_png_to_jpg(test_filenames[9].first);
	img_rgb8_png_to_jpg.WriteImgToFile(test_filenames[9].second, image::JPEG);
	image::Image img_rgb8_jpg_to_jpg(test_filenames[10].first);
	img_rgb8_jpg_to_jpg.WriteImgToFile(test_filenames[10].second, image::JPEG);
	image::Image img_rgb8_bmp_to_jpg(test_filenames[11].first);
	img_rgb8_bmp_to_jpg.WriteImgToFile(test_filenames[11].second, image::JPEG);

	image::Image img_rgb8_webp_to_png(test_filenames[12].first);
	img_rgb8_webp_to_png.WriteImgToFile(test_filenames[12].second, image::PNG);
	image::Image img_rgb8_png_to_png(test_filenames[13].first);
	img_rgb8_png_to_png.WriteImgToFile(test_filenames[13].second, image::PNG);
	image::Image img_rgb8_jpg_to_png(test_filenames[14].first);
	img_rgb8_jpg_to_png.WriteImgToFile(test_filenames[14].second, image::PNG);
	image::Image img_rgb8_bmp_to_png(test_filenames[15].first);
	img_rgb8_bmp_to_png.WriteImgToFile(test_filenames[15].second, image::PNG);

	auto time_stop = std::chrono::high_resolution_clock::now();
	auto time_delta = time_stop - time_start;

	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_delta).count();
}

void test_gif() {
  utils::DecodeGif gif((std::string)"test\\bigger.gif");
  //utils::DecodeGif gif((std::string)"test\\unanimated.gif");

  const auto width = gif.GetInfos().width;
  const auto height = gif.GetInfos().height;

  //const auto image = gif.Next();

  //std::cout << "gif " << (uint64_t)image << std::endl;
  const auto size = gif.GetInfos().height * gif.GetInfos().width * 4;
  //std::vector<char> out;
  //out.reserve(size);

  auto img_count = 0;

  while (true) {
    auto image = gif.Next();
    if (image == nullptr) {
      break;
    }
    std::string s = "frame";
    s += std::to_string(img_count);
    s += ".txt";
    utils::FileIO::WriteToFile((char*)image, s, size);
    img_count++;
  }
}

int main(int argc, const char* argv[]) {
  auto start = std::chrono::system_clock::now();
  auto end = std::chrono::system_clock::now();
  auto diff = std::chrono::duration_cast <std::chrono::milliseconds> (end - start).count();

  //testAnimWebp();
  image::Image img_rgb8_gif_to_webp((std::string)"test\\navmap.gif");
  img_rgb8_gif_to_webp.WriteImgToFile((std::string)"navmap.webp", image::WEBP);

  std::cout << "Done" << std::endl;

  end = std::chrono::system_clock::now();
  diff = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
  std::cout << "Time elapsed: " << diff << "ms" << std::endl;

	std::cin.get();
	return 0;
}
