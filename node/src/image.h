#pragma once

#include <napi.h>
#include <image.h>
#include "async_workers.hpp"

namespace cpp_image = image;

class Image : public Napi::ObjectWrap<Image> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Value Algorithm(const Napi::CallbackInfo& info);
  Image(const Napi::CallbackInfo& info);
  ~Image();

private:
  Napi::Value WriteToFile(const Napi::CallbackInfo& info);
  void WriteToFileSync(const Napi::CallbackInfo& info);
  Napi::Value RescaleSync(const Napi::CallbackInfo& info);

  // filename and format are out parameters, returns false on error
  static bool GetOutInfos(const Napi::CallbackInfo& info, std::string* filename, image::ImgFormat* format);
  // will act as an enum
  cpp_image::Image* img_;
};
