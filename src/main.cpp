#include "pch.h"
#include "decode.h"
#include "gil.hpp"
#include "gil/extension/io/png.hpp"
#include "gil/extension/io/jpeg.hpp"
#include "gil/extension/io/bmp.hpp"

#include <vector>

namespace gil = boost::gil;

int main(int argc, const char* argv[]) {

	//std::ifstream stream(argv[1], std::ios::binary);

	int length = 0;
	
	auto data = utils::FileIO::getDataFromFile("1_webp_ll.webp", &length);
	utils::FileIO::writeToFile(data, "test2.webp", length);

	int width = 0;
	int height = 0;

	const auto info = WebPGetInfo(reinterpret_cast<uint8_t*>(data), length, &width, &height);

	gil::rgb8_image_t img(100, 100);
	auto v = gil::view(img);
	auto b = v.begin();
	while (b != v.end())
	{
		*b = gil::rgb8_pixel_t{ 255, 128, 0 };
		b++;
	}
	gil::write_view("image.png", v, gil::png_tag());

	auto rgb = WebPDecodeRGB(reinterpret_cast<uint8_t*>(data), length, &width, &height);

	std::cout << std::endl << info << " " << width << " " << height;

	std::vector<gil::rgb8_pixel_t> buffer;

	for (auto i = 0; i < 361200; i=i+3)
	{
		//							r				g				b
		buffer.push_back({ *(rgb + i), *(rgb + i + 1), *(rgb + i + 2) });
	}

	std::cout << std::endl << "vector length: " << buffer.size();

	auto view2 = gil::interleaved_view(400, 
		301,
		&buffer[0],
		//buffer,
		1200
		);

	gil::write_view("test_from_webp.png", view2, gil::png_tag());
	gil::write_view("test_from_webp.jpg", view2, gil::jpeg_tag());
	gil::write_view("test_from_webp.bmp", view2, gil::bmp_tag());

	WebPBitstreamFeatures features;
	auto result = WebPGetFeatures(reinterpret_cast<uint8_t*>(data), length, &features);

	// VP8StatusCode enum wrappen and errors ausgeben

	std::cout << std::endl << result << " " << features.width << " " << features.height;
	
	std::cin.get();
	return 0;
}
