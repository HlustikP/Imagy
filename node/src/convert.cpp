#include "convert.h"

Napi::Object Convert(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
	const auto image_infos = info[0].As<Napi::Object>();

	image_infos.Set("error", false);

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

	if (image_infos.Has("image")) {
		in_filename = std::string(image_infos.Get("image").As<Napi::String>());
	} else {
		Napi::TypeError::New(env, "\'image\' property not set").ThrowAsJavaScriptException();
		return Napi::Object::New(env);
	}

	image::ImgFormat format;

	if (image_infos.Has("outName")) {
		out_filename = std::string(image_infos.Get("outName").As<Napi::String>());
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

	image_infos.Set("finished", true);

	switch (result) {
		case 0:
			break;
		case 1:
			image_infos.Set("error", true);
			Napi::Error::New(env, "Error while encoding").ThrowAsJavaScriptException();
			return image_infos;
		case 2:
			image_infos.Set("error", true);
			Napi::Error::New(env, "Error when trying to infer file format from outFile").ThrowAsJavaScriptException();
			return image_infos;
	}

  return image_infos;
}

Napi::Promise ConvertAsync(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	auto deferred = Napi::Promise::Deferred::New(info.Env());
	Napi::Object image_infos(env, info[0].As<Napi::Object>());

	image_infos.Set("error", false);

	if (info.Length() < 1 || info.Length() > 2) {
		Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
		deferred.Reject(Napi::Number::New(env, 0));
		return deferred.Promise();
	}

	if (!image_infos.IsObject()) {
		Napi::TypeError::New(env, "Wrong argument type, expected Object").ThrowAsJavaScriptException();
		deferred.Reject(Napi::Number::New(env, 0));
		return deferred.Promise();
	}

	std::string in_filename;
	std::string out_filename;

	if (image_infos.Has("image")) {
		in_filename = std::string(image_infos.Get("image").As<Napi::String>());
	}
	else {
		Napi::TypeError::New(env, "\'image\' property not set").ThrowAsJavaScriptException();
		deferred.Reject(Napi::Number::New(env, 0));
		return deferred.Promise();
	}

	image::ImgFormat format;

	if (image_infos.Has("outName")) {
		out_filename = std::string(image_infos.Get("outName").As<Napi::String>());
		format = image::Image::GetFileExtension(out_filename);
		if (format == image::ImgFormat::INVALID) {
			Napi::Error::New(env, "Cannot infer file format from outName extension").ThrowAsJavaScriptException();
			deferred.Reject(Napi::Number::New(env, 0));
			return deferred.Promise();
		}
	}
	else {
		Napi::Error::New(env, "\'outName\' property not set").ThrowAsJavaScriptException();
		deferred.Reject(Napi::Number::New(env, 0));
		return deferred.Promise();
	}

	ConversionWorker* conversion_worker = new ConversionWorker(env, in_filename, out_filename, format);
	auto promise = conversion_worker->GetPromise();
	conversion_worker->Queue();

	return promise;
}
