#include "image.h"

Napi::Object Image::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func =
    DefineClass(env,
      "Image",
      { InstanceMethod("writeToFileSync", &Image::WriteToFileSync),
        InstanceMethod("writeToFile", &Image::WriteToFile),
        InstanceMethod("rescaleSync", &Image::RescaleSync),
        InstanceMethod("rescale", &Image::Rescale),
        InstanceMethod("flipSync", &Image::FlipSync),
        InstanceMethod("flipDSync", &Image::FlipDSync),
        InstanceMethod("flipHSync", &Image::FlipHSync),
        InstanceMethod("flipVSync", &Image::FlipVSync),
        StaticAccessor<&Image::Algorithm>("Algorithm")
      });

  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);

  exports.Set("Image", func);
  return exports;
}

Image::Image(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Image>(info) {
  Napi::Env env = info.Env();
  const int length = info.Length();

  if (length <= 0 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Image constructor exptected String").ThrowAsJavaScriptException();
    return;
  }

  auto filename = std::string(info[0].As<Napi::String>());;
  img_ = new cpp_image::Image(filename);
}

Image::~Image() {
    delete img_;
}

Napi::Value Image::WriteToFile(const Napi::CallbackInfo& info) {
  std::string out_file;
  cpp_image::ImgFormat format;

  auto env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);

  if (!GetOutInfos(info, &out_file, &format)) {
    auto error = Napi::Error::New(info.Env(), "Cannot infer file format from output file string");
    deferred.Reject(error.Value());
    return deferred.Promise();
  }

  ConversionWorker* conversion_worker = new ConversionWorker(env, img_, out_file, format);
  auto promise = conversion_worker->GetPromise();
  conversion_worker->Queue();

  return promise;
}

void Image::WriteToFileSync(const Napi::CallbackInfo& info) {
  std::string out_file;
  cpp_image::ImgFormat format;

  if (!GetOutInfos(info, &out_file, &format)) {
    Napi::Error::New(info.Env(), "Cannot infer file format from output file string").ThrowAsJavaScriptException();
    return;
  }

  auto result = img_->WriteImgToFile(out_file, format);

  switch (result) {
  case 0:
    break;
  case 1:
    Napi::Error::New(info.Env(), "Error while decoding").ThrowAsJavaScriptException();
    break;
  case 2:
    Napi::Error::New(info.Env(), "Error when trying to infer file format from outFile").ThrowAsJavaScriptException();
    break;
  }
}

Napi::Value Image::RescaleSync(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  if (info.Length() < 2) {
    Napi::TypeError::New(env, "Wrong number of arguments, needs at least height and witdh").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(env, "Wrong argument type, height must be of type Number").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }
  auto height = info[0].As<Napi::Number>();

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(env, "Wrong argument type, width must be of type Number").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }
  auto width = info[1].As<Napi::Number>();
  
  // Set default to BILINEAR
  auto algorithm = (int)cpp_image::InterpolationAlgorithms::BILINEAR;

  if (info.Length() > 2) {
    if (!info[2].IsNumber()) {
      Napi::Error::New(env, "Algorithm not found, use one from the Algorithm enum").ThrowAsJavaScriptException();
      return Napi::Object::New(env);
    }
    algorithm = info[2].As<Napi::Number>();
  }

  img_->ChangeScale(height.Int32Value(),
    width.Int32Value(),
    static_cast<cpp_image::InterpolationAlgorithms>(algorithm)
  );

  return info.This();
}

Napi::Value Image::Rescale(const Napi::CallbackInfo& info) {
  auto env = info.Env();
  auto deferred = Napi::Promise::Deferred::New(env);

  if (info.Length() < 2) {
    auto error = Napi::TypeError::New(env, "Wrong number of arguments, needs at least height and witdh");
    deferred.Reject(error.Value());
    return deferred.Promise();
  }

  if (!info[0].IsNumber()) {
    auto error = Napi::TypeError::New(env, "Wrong argument type, height must be of type Number");
    deferred.Reject(error.Value());
    return deferred.Promise();
  }
  auto height = info[0].As<Napi::Number>();

  if (!info[1].IsNumber()) {
    auto error = Napi::TypeError::New(env, "Wrong argument type, width must be of type Number");
    deferred.Reject(error.Value());
    return deferred.Promise();
  }
  auto width = info[1].As<Napi::Number>();

  // Set default to BILINEAR
  auto algorithm = (int)cpp_image::InterpolationAlgorithms::BILINEAR;

  if (info.Length() > 2) {
    if (!info[2].IsNumber()) {
      auto error = Napi::Error::New(env, "Algorithm not found, use one from the Algorithm enum");
      deferred.Reject(error.Value());
      return deferred.Promise();
    }
    algorithm = info[2].As<Napi::Number>();
  }

  ResizingWorker* conversion_worker = new ResizingWorker(info.This(),
    env, 
    img_, 
    height.Int32Value(),
    width.Int32Value(),
    (imagy::InterpolationAlgorithms)algorithm
  );

  auto promise = conversion_worker->GetPromise();
  conversion_worker->Queue();

  return promise;
}

Napi::Value Image::FlipDSync(const Napi::CallbackInfo& info)
{
  img_->FlipD();

  return info.This();
}

Napi::Value Image::FlipHSync(const Napi::CallbackInfo& info)
{
  img_->FlipH();

  return info.This();
}

Napi::Value Image::FlipVSync(const Napi::CallbackInfo& info)
{
  img_->FlipV();

  return info.This();
}

Napi::Value Image::FlipSync(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  if (info.Length() < 1) {
    Napi::TypeError::New(env, "Wrong number of arguments, needs at least one").ThrowAsJavaScriptException();
    return Napi::Object::New(env);
  }

  auto direction = info[0].As<Napi::String>().ToString().Utf8Value();

  if (direction == "D" || direction == "d") {
    FlipDSync(info);
  } else if (direction == "H" || direction == "h") {
    FlipHSync(info);
  } else if (direction == "V" || direction == "v") {
    FlipVSync(info);
  } else {
    Napi::TypeError::New(env, "Cannot deduce flip axis from given string input").ThrowAsJavaScriptException();
  }

  return info.This();
}

Napi::Value Image::Algorithm(const Napi::CallbackInfo& info) {
  // Setup Alogrithm "enum"
  auto Algorithm = Napi::Object::New(info.Env());
  Algorithm.Set("NEAREST_NEIGHBOUR", (int)cpp_image::InterpolationAlgorithms::NEAREST_NEIGHBOUR);
  Algorithm.Set("BILINEAR", (int)cpp_image::InterpolationAlgorithms::BILINEAR);

  return Algorithm;
}

bool Image::GetOutInfos(const Napi::CallbackInfo& info, std::string* filename, cpp_image::ImgFormat* format) {
  *filename = std::string(info[0].As<Napi::String>());;
  *format = cpp_image::Image::GetFileExtension(*filename);

  return !(*format == cpp_image::ImgFormat::INVALID);
}
