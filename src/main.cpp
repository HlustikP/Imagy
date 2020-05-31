#include "pch.h"
#include "decode.h"
#include "gil.hpp"
#include "gil/extension/io/png.hpp"
#include "gil/extension/io/jpeg.hpp"
#include "gil/extension/io/bmp.hpp"

#include <vector>
#include <iterator>

namespace gil = boost::gil;

int main(int argc, const char* argv[]) {

	std::string filename = "1_webp_ll.webp";
	std::string out = "test_gubbel.webp";

	image::Image img(filename);
	img.WriteImgToFile(out, image::WEBP);
	
	/*
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

	auto* rgb = WebPDecodeRGB(reinterpret_cast<uint8_t*>(data), length, &width, &height);

	std::cout << std::endl << info << " " << width << " " << height;

	const auto view2 = gil::interleaved_view(
		400, 
		301,
		reinterpret_cast<gil::rgb8_pixel_t*>(rgb),
		1200
		);

	gil::write_view("test_from_webp.png", view2, gil::png_tag());
	gil::write_view("test_from_webp.jpg", view2, gil::jpeg_tag());
	gil::write_view("test_from_webp.bmp", view2, gil::bmp_tag());

	WebPBitstreamFeatures features;
	const auto result = WebPGetFeatures(reinterpret_cast<uint8_t*>(data), length, &features);

	// VP8StatusCode enum wrappen and errors ausgeben

	std::cout << std::endl << result << " " << features.width << " " << features.height;
	*/
	
	std::cin.get();
	return 0;
}
