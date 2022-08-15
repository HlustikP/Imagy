#include "utils.h"
#include <iostream>

// returns the index of the valid signature on success and -1 on error
int validate_arguments(const Napi::CallbackInfo& info, std::vector<std::vector<napi_valuetype>>& function_signatures) {
  Napi::Env env = info.Env();

  bool size_error = false;
  bool type_error = false;

  //for (auto& signature : function_signatures) {
  for (unsigned int i = 0; i < function_signatures.size(); i++) {
    const auto size = function_signatures[i].size();
    if (size != info.Length()) {
      if (function_signatures.size() == 1) {
        Napi::Error::New(env, "Wrong number of arguments, expected: " + std::to_string(size)).ThrowAsJavaScriptException();
        return -1;
      }

      size_error = true;
      continue;
    }

    int arg_index = 0;
    auto local_type_error = false;
    for (auto& parameter : function_signatures[i]) {
      if (parameter != info[arg_index].Type()) {
        type_error = true;
        local_type_error = true;
        break;
      }

      arg_index++;
    }
    if (!local_type_error) {
      return i;
    }
  }

  if (size_error || type_error) {
    Napi::Error::New(env, "Wrong number of arguments or wrong argument types provided.").ThrowAsJavaScriptException();
    return -1;
  }

  // Should be unreachable
  return -2;
}
