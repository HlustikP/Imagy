#include "image.h"

Napi::Object Image::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func =
        DefineClass(env,
            "Image",
            { InstanceMethod("writeToFileSync", &Image::WriteToFileSync),
                       InstanceMethod("writeToFile", &Image::WriteToFile)
            });

    Napi::FunctionReference* constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Image", func);
    return exports;
}

Image::Image(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<Image>(info) {
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

void Image::WriteToFile(const Napi::CallbackInfo& info) {
    std::string out_file;
    image::ImgFormat format;

    auto env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);

    if (!GetOutInfos(info, &out_file, &format)) {
        auto error = Napi::Error::New(info.Env(), "Cannot infer file format from output file string");
        deferred.Reject(error.Value());
        return;
    }

    ConversionWorker* conversion_worker = new ConversionWorker(env, img_, out_file, format);
    auto promise = conversion_worker->GetPromise();
    conversion_worker->Queue();
}

void Image::WriteToFileSync(const Napi::CallbackInfo& info) {
    std::string out_file;
    image::ImgFormat format;

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

bool Image::GetOutInfos(const Napi::CallbackInfo& info, std::string* filename, image::ImgFormat* format) {
    *filename = std::string(info[0].As<Napi::String>());;
    *format = image::Image::GetFileExtension(*filename);

    return !(*format == image::ImgFormat::INVALID);
}
