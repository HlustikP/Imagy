#include "convert.h"

Napi::Object Convert(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  const auto argument_index = ValidateConvertSignatures(info);
  auto out_object = Napi::Object::New(env);
  out_object.Set("error", false);

  if (argument_index == -1) {
    out_object.Set("error", true);
    return out_object;
  }

  std::string in_filename;
  std::string out_filename;
  imagy::ImgFormat format = imagy::ImgFormat::INVALID;

  if (argument_index == SIGNATURE_OBJECT) {
    const auto image_infos = info[0].As<Napi::Object>();
    if (info[0].As<Napi::Object>().Has("image")) {
      in_filename = std::string(image_infos.Get("image").As<Napi::String>());
    }
    else {
      Napi::Error::New(env, "\'image\' property not set").ThrowAsJavaScriptException();
      out_object.Set("error", true);
      return out_object;
    }

    if (image_infos.Has("outName")) {
      out_filename = std::string(image_infos.Get("outName").As<Napi::String>());
      format = imagy::Image::GetFileExtension(out_filename);
      if (format == imagy::ImgFormat::INVALID) {
        Napi::Error::New(env, "Cannot infer file format from outName extension").ThrowAsJavaScriptException();
        out_object.Set("error", true);
        return out_object;
      }
    }
    else {
      Napi::Error::New(env, "\'outName\' property not set").ThrowAsJavaScriptException();
      out_object.Set("error", true);
      return out_object;
    }
  }

  if (argument_index == SIGNATURE_STRING_STRING) {
    in_filename = std::string(info[0].As<Napi::String>());
    out_filename = std::string(info[1].As<Napi::String>());

    format = imagy::Image::GetFileExtension(out_filename);
    if (format == imagy::ImgFormat::INVALID) {
      Napi::Error::New(env, "Cannot infer file format from outName extension").ThrowAsJavaScriptException();
      out_object.Set("error", true);
      return out_object;
    }
  }

  imagy::Image img(in_filename);
  const auto result = img.WriteImgToFile(out_filename, format);

  out_object.Set("finished", true);

	switch (result) {
		case 0:
			break;
		case 1:
      out_object.Set("error", true);
			Napi::Error::New(env, "Error while encoding").ThrowAsJavaScriptException();
			return out_object;
		case 2:
      out_object.Set("error", true);
			Napi::Error::New(env, "Error when trying to infer file format from outFile").ThrowAsJavaScriptException();
			return out_object;
	}

  return out_object;
}

Napi::Promise ConvertAsync(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	auto deferred = Napi::Promise::Deferred::New(info.Env());

  const auto argument_index = ValidateConvertSignatures(info);

	std::string in_filename;
	std::string out_filename;
  imagy::ImgFormat format = imagy::ImgFormat::INVALID;

  if (argument_index == SIGNATURE_OBJECT) {
    const auto image_infos = info[0].As<Napi::Object>();
    if (image_infos.Has("image")) {
      in_filename = std::string(image_infos.Get("image").As<Napi::String>());
    }
    else {
      const auto err = Napi::Error::New(env, "\'image\' property not set");
      err.ThrowAsJavaScriptException();
      deferred.Reject(err.Value());
      return deferred.Promise();
    }

    if (image_infos.Has("outName")) {
      out_filename = std::string(image_infos.Get("outName").As<Napi::String>());
      format = imagy::Image::GetFileExtension(out_filename);
      if (format == imagy::ImgFormat::INVALID) {
        const auto err = Napi::Error::New(env, "Cannot infer file format from outName extension");
        err.ThrowAsJavaScriptException();
        deferred.Reject(err.Value());
        return deferred.Promise();
      }
    }
    else {
      const auto err = Napi::Error::New(env, "\'outName\' property not set");
      err.ThrowAsJavaScriptException();
      deferred.Reject(err.Value());
      return deferred.Promise();
    }
  }

  if (argument_index == SIGNATURE_STRING_STRING) {
    in_filename = std::string(info[0].As<Napi::String>());
    out_filename = std::string(info[1].As<Napi::String>());

    format = imagy::Image::GetFileExtension(out_filename);
    if (format == imagy::ImgFormat::INVALID) {
      Napi::Error::New(env, "Cannot infer file format from outName extension").ThrowAsJavaScriptException();
    }
  }

	ConversionWorker* conversion_worker = new ConversionWorker(env, in_filename, out_filename, format);
	auto promise = conversion_worker->GetPromise();
	conversion_worker->Queue();

	return promise;
}

inline int ValidateConvertSignatures(const Napi::CallbackInfo& info) {
  // Object as a config
  napi_valuetype arg00 = napi_object;

  // Input + Output file strings
  napi_valuetype arg10 = napi_string;
  napi_valuetype arg11 = napi_string;

  std::vector<napi_valuetype> sig0 = { arg00 };
  std::vector<napi_valuetype> sig1 = { arg10, arg11 };
  std::vector<std::vector<napi_valuetype>> func_sigs = { sig0, sig1 };

  return validate_arguments(info, func_sigs);
}
