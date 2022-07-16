#pragma once

#include <string>

namespace test {
const struct hashing {
  inline static std::string test_string = "hello world";
  inline static std::string target_hash = "b94d27b9934d3e08a52e52d7da7dabfac484efe37a5380ee9088f7ace2efcde9";
};

const struct fileio {
  inline static std::string test_file = "../../../node/media/cat.jpg";
  inline static std::string target_hash = "ebb9977689f4215cea72b4373c2d2b80c21bf17fa30228f9fb9e14f77ca06441";
};

const struct jpeg {
  inline static std::string test_file = "../../../node/media/cat.jpg";
  inline static std::string target_file = "../../../node/media_output/cat.jpg";
  inline static std::string target_hash = "ebb9977689f4215cea72b4373c2d2b80c21bf17fa30228f9fb9e14f77ca06441";
};

const struct bmp {
  inline static std::string test_file = "../../../node/media/cat.bmp";
  inline static std::string target_file = "../../../node/media_output/cat.bmp";
  inline static std::string target_hash = "06242e907d5f1f526ca2a092256baf4ba9a82deb42ecc4d740d89b9f4d0a2260";
};

const struct png {
  inline static std::string test_file = "../../../node/media/cat.png";
  inline static std::string target_file = "../../../node/media_output/cat.png";
  inline static std::string target_hash = "e6c9ab5f95cc86ff74304301c4bda5abfe48743516f38f690f65bc7d26fa9c9d";
};
}
