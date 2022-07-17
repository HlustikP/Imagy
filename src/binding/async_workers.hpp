#pragma once

#include <napi.h>
#include <imagy.h>

// Implementation of the Napi::AsyncWorker abstract class for the image conversion function
class ConversionWorker : public Napi::AsyncWorker {
public:
	ConversionWorker(Napi::Env& env, std::string in_filename, std::string out_filename, imagy::ImgFormat format)
		: Napi::AsyncWorker(env), deferred_(Napi::Promise::Deferred::New(env)), in_filename_(in_filename), out_filename_(out_filename), format_(format) {
	}

  ConversionWorker(Napi::Env& env, imagy::Image* img, std::string out_filename, imagy::ImgFormat format)
    : Napi::AsyncWorker(env), deferred_(Napi::Promise::Deferred::New(env)), img_(img), out_filename_(out_filename), format_(format) {
  }

	~ConversionWorker() {}

	// Executed inside a new thread
	void Execute() {
    auto img = img_ == nullptr ? new imagy::Image(in_filename_) : img_;
		const auto result = img->WriteImgToFile(out_filename_, format_);

    if (img_ == nullptr) {
        delete img;
    }

		switch (result) {
		case 0:
			break;
		case 1:
			Napi::AsyncWorker::SetError("Error while decoding");
			break;
		case 2:
			Napi::AsyncWorker::SetError("Error when trying to infer file format from outFile");
			break;
		}
	}

	// Executed when the async work is completed, this function will be put into the main event loop
	void OnOK() {
		auto result = Napi::Object::New(Env());
		auto img = Napi::Object::New(Env());
		result.Set("finished", true);
		result.Set("error", false);
		result.Set("img", img);

		deferred_.Resolve(result);
	}

	// Same as above but executed when something inside Execute() throws an error via Napi::AsyncWorker::SetError or std::runtime_error
	void OnError(Napi::Error const& error) {
		deferred_.Reject(error.Value());
	}

	Napi::Promise GetPromise() { 
		return deferred_.Promise(); 
	}

private:
  Napi::Promise::Deferred deferred_;
	std::string in_filename_;
  imagy::Image* img_ = nullptr;
	std::string out_filename_;
	imagy::ImgFormat format_;
};
