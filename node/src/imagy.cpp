#include <napi.h>
#include "convert.h"
#include "image.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "convertSync"), Napi::Function::New(env, Convert));
  exports.Set(Napi::String::New(env, "quickConvertSync"), Napi::Function::New(env, Convert));
  exports.Set(Napi::String::New(env, "convert"), Napi::Function::New(env, ConvertAsync));
  exports.Set(Napi::String::New(env, "quickConvert"), Napi::Function::New(env, ConvertAsync));
  Image::Init(env, exports);
  return exports;
}

NODE_API_MODULE(imagy, Init)
