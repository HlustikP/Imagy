#include <napi.h>
#include "convert.h"

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "convert"), Napi::Function::New(env, Convert));
  exports.Set(Napi::String::New(env, "quickConvert"), Napi::Function::New(env, Convert));
  exports.Set(Napi::String::New(env, "convertAsync"), Napi::Function::New(env, ConvertAsync));
  exports.Set(Napi::String::New(env, "quickConvertAsync"), Napi::Function::New(env, ConvertAsync));
  return exports;
}

NODE_API_MODULE(imagy, Init)
