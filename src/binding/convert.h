#pragma once

#include <napi.h>
#include <imagy.h>

#include "async_workers.hpp"
#include "utils.h"

const auto SIGNATURE_OBJECT = 0;
const auto SIGNATURE_STRING_STRING = 1;

Napi::Object Convert(const Napi::CallbackInfo& info);
Napi::Promise ConvertAsync(const Napi::CallbackInfo& info);
inline int ValidateConvertSignatures(const Napi::CallbackInfo& info);
