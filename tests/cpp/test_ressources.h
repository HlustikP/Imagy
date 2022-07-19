#pragma once

#include <string>

namespace test {
const struct hashing {
  inline static std::string test_string = "hello world";
  inline static std::string target_hash = "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9";
};

const struct fileio {
  inline static std::string test_file = "../../../tests/media/cat.jpg";
  inline static std::string target_hash = "ebb9977689f4215cea72b4373c2d2b80c21bf17fa30228f9fb9e14f77ca06441";
};

const struct jpeg {
  inline static std::string test_file = "../../../tests/media/cat.jpg";
  inline static std::string target_file = "../../../tests/media_output/cat.jpg";
  inline static std::string target_hash = "c5fcd03872f66d4d361f2da9241410e17224321ce563c88b355141b2d3030a07";
};

const struct bmp {
  inline static std::string test_file = "../../../tests/media/cat.bmp";
  inline static std::string target_file = "../../../tests/media_output/cat.bmp";
  inline static std::string target_hash = "145eba200a44556f626ba4cbf60c4218dedbe44ef66573beb30532f19024ecd9";
};

const struct png {
  inline static std::string test_file = "../../../tests/media/cat.png";
  inline static std::string target_file = "../../../tests/media_output/cat.png";
  inline static std::string target_hash = "6c18ae9a2cf06365304d246f09be7389406dbaa09d84199d5fa0786f9375a67c";
};

const struct flip_d {
  inline static std::string test_file = "../../../tests/media/cat.jpg";
  inline static std::string target_file = "../../../tests/media_output/flipped_cat_d.jpg";
  inline static std::string target_hash = "66d36ff192b7eea30f5d0725efba0e6f74e08a942d84de7453507d005727a211";
};

const struct flip_h {
  inline static std::string test_file = "../../../tests/media/cat.jpg";
  inline static std::string target_file = "../../../tests/media_output/flipped_cat_h.jpg";
  inline static std::string target_hash = "3598e867e4afa5093a602d0f4ce721ff9626fa391a46b0ad47b121722dfec1f1";
};

const struct flip_v {
  inline static std::string test_file = "../../../tests/media/cat.jpg";
  inline static std::string target_file = "../../../tests/media_output/flipped_cat_v.jpg";
  inline static std::string target_hash = "f31fc7bfafba707dfffb814f3aeaa36de99ca9b10916fc005599987e9591df30";
};

}
