#pragma once

#include <napi.h>
#include <imagy.h>
#include "async_workers.hpp"

namespace cpp_image = imagy;

class Image : public Napi::ObjectWrap<Image> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  // will act as an enum
  static Napi::Value Algorithm(const Napi::CallbackInfo& info);
  Image(const Napi::CallbackInfo& info);
  ~Image();

private:
  Napi::Value WriteToFile(const Napi::CallbackInfo& info);
  void WriteToFileSync(const Napi::CallbackInfo& info);
  Napi::Value RescaleSync(const Napi::CallbackInfo& info);
  Napi::Value Rescale(const Napi::CallbackInfo& info);
  Napi::Value FlipDSync(const Napi::CallbackInfo& info);
  Napi::Value FlipHSync(const Napi::CallbackInfo& info);
  Napi::Value FlipVSync(const Napi::CallbackInfo& info);
  Napi::Value FlipSync(const Napi::CallbackInfo& info);

  // filename and format are out parameters, returns false on error
  static bool GetOutInfos(const Napi::CallbackInfo& info, std::string* filename, cpp_image::ImgFormat* format);
  cpp_image::Image* img_;
};
