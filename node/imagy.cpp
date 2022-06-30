#include <napi.h>
#include "image.h"

Napi::Object Convert(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
	const auto arg0 = info[0].As<Napi::Object>();

	arg0.Set("error", false);

  if (info.Length() < 1 || info.Length() > 2) {
    Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }

  if (!info[0].IsObject()) {
    Napi::TypeError::New(env, "Wrong argument type, expected Object").ThrowAsJavaScriptException();
		return Napi::Object::New(env);
  }

	std::string in_filename;
	std::string out_filename;

	if (arg0.Has("image")) {
		in_filename = std::string(arg0.Get("image").As<Napi::String>());
	} else {
		Napi::TypeError::New(env, "\'image\' property not set").ThrowAsJavaScriptException();
		return Napi::Object::New(env);
	}

	image::ImgFormat format;

	if (arg0.Has("outName")) {
		out_filename = std::string(arg0.Get("outName").As<Napi::String>());
		format = image::Image::GetFileExtension(out_filename);
		if (format == image::ImgFormat::INVALID) {
			Napi::Error::New(env, "Cannot infer file format from outName extension").ThrowAsJavaScriptException();
			return Napi::Object::New(env);
		}
	} else {
		Napi::Error::New(env, "\'outName\' property not set").ThrowAsJavaScriptException();
		return Napi::Object::New(env);
	}

  image::Image img(in_filename);
  const auto result = img.WriteImgToFile(out_filename, format);

	arg0.Set("finished", true);

	switch (result) {
		case 0:
			break;
		case 1:
			arg0.Set("error", true);
			Napi::Error::New(env, "Error while encoding").ThrowAsJavaScriptException();
			return arg0;
		case 2:
			arg0.Set("error", true);
			Napi::Error::New(env, "Error when trying to infer file format from outFile").ThrowAsJavaScriptException();
			return arg0;
	}

  return arg0;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.Set(Napi::String::New(env, "convert"), Napi::Function::New(env, Convert));
  return exports;
}

NODE_API_MODULE(imagy, Init)
