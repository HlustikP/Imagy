#pragma once

#include <napi.h>

int validate_arguments(const Napi::CallbackInfo& info, std::vector<std::vector<napi_valuetype>>& function_signatures);
