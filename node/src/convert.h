#pragma once

#include <napi.h>
#include <image.h>

#include "async_workers.hpp"

Napi::Object Convert(const Napi::CallbackInfo& info);
Napi::Promise ConvertAsync(const Napi::CallbackInfo& info);
