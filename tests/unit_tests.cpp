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
/*
TEST(Image, Jpeg) {
  int length = 0;
  std::string file_content;
  auto volatile format = imagy::ImgFormat::BMP;
  //img.WriteImgToFile(test::jpeg::target_file, imagy::ImgFormat::JPEG);
  //auto output_img_data = utils::FileIO::GetDataFromFile(test::jpeg::target_file, &length);

  //file_content.assign(output_img_data, length);

  auto test_string = std::string("");// hashString(file_content);

  EXPECT_EQ(test_string, test::jpeg::target_hash);
  //delete[] output_img_data;
}

TEST(Image, Bmp) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::bmp::test_file);
  img.WriteImgToFile(test::bmp::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::bmp::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = std::string("");// hashString(file_content);

  EXPECT_EQ(test_string, test::bmp::target_hash);
  delete[] output_img_data;
}

TEST(Image, Png) {
  int length = 0;
  std::string file_content;
  imagy::Image img(test::png::test_file);
  img.WriteImgToFile(test::png::target_file, imagy::ImgFormat::JPEG);
  auto output_img_data = utils::FileIO::GetDataFromFile(test::png::target_file, &length);

  file_content.assign(output_img_data, length);

  auto test_string = std::string("");// hashString(file_content);

  EXPECT_EQ(test_string, test::png::target_hash);
  delete[] output_img_data;
}
*/
