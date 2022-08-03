#include <gtest/gtest.h>
#include "imagy.h"
#include "SHA256.h"
#include "test_ressources.h"

std::string hashString(std::string& input) {
  SHA256 sha;
  sha.update(input);
  auto digest = sha.digest();
  auto test_string = SHA256::toString(digest);

  delete[] digest;
  return test_string;
}

TEST(Utils, Hashing) {
  SHA256 sha;
  sha.update(test::hashing::test_string);
  auto digest = sha.digest();
  auto test_string = SHA256::toString(digest);

  EXPECT_EQ(test_string, test::hashing::target_hash);
  delete[] digest;
}

TEST(Utils, FileIO) {
  int length = 0;
  std::string file_content;
  auto data = utils::FileIO::GetDataFromFile(test::fileio::test_file, &length);
  file_content.assign(data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::fileio::target_hash);
  delete[] data;
}

TEST(Image, Jpeg) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::jpeg::test_file);
  img.WriteImgToFile(test::jpeg::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::jpeg::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::jpeg::target_hash);
  delete[] output_img_data;
}

TEST(Image, Bmp) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::bmp::test_file);
  img.WriteImgToFile(test::bmp::target_file, imagy::ImgFormat::BMP);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::bmp::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::bmp::target_hash);
  delete[] output_img_data;
}

TEST(Image, Png) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::png::test_file);
  img.WriteImgToFile(test::png::target_file, imagy::ImgFormat::PNG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::png::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::png::target_hash);
  delete[] output_img_data;
}

TEST(Image, Tiff) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::tiff::test_file);
  img.WriteImgToFile(test::tiff::target_file, imagy::ImgFormat::TIFF);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::tiff::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::tiff::target_hash);
  delete[] output_img_data;
}

TEST(Manipulation, FlipD) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::flip_d::test_file);
  img.FlipD();
  img.WriteImgToFile(test::flip_d::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::flip_d::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::flip_d::target_hash);
  delete[] output_img_data;
}

TEST(Manipulation, FlipH) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::flip_h::test_file);
  img.FlipH();
  img.WriteImgToFile(test::flip_h::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::flip_h::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::flip_h::target_hash);
  delete[] output_img_data;
}

TEST(Manipulation, FlipV) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::flip_v::test_file);
  img.FlipV();
  img.WriteImgToFile(test::flip_v::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::flip_v::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = hashString(file_content);

  EXPECT_EQ(test_string, test::flip_v::target_hash);
  delete[] output_img_data;
}
