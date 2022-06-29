#include <napi.h>
#include "image.h"

Napi::String Convert(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
	const auto arg0 = info[0].As<Napi::String>();

  if (info.Length() < 1 || info.Length() > 2) {
    Napi::TypeError::New(env, "Wrong number of arguments")
        .ThrowAsJavaScriptException();
    return arg0;
  }

  if (!info[0].IsString()) {
    Napi::TypeError::New(env, "Wrong argument type").ThrowAsJavaScriptException();
		return arg0;
  }

  image::Image img_rgb8_gif_to_webp((std::string)"media/interlacing.gif");
  img_rgb8_gif_to_webp.WriteImgToFile((std::string)"media_output/interlacing.webp", image::WEBP);

  return arg0;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "convert"), Napi::Function::New(env, Convert));
  return exports;
}

NODE_API_MODULE(addon, Init)
