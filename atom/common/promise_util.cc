// Copyright (c) 2018 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <string>

#include "atom/common/api/locker.h"
#include "atom/common/promise_util.h"

namespace atom {

namespace util {

Promise::Promise(v8::Isolate* isolate) {
  auto context = isolate->GetCurrentContext();
  auto resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
  isolate_ = isolate;

  context_.Reset(isolate, context);
  resolver_.Reset(isolate, resolver);
}

Promise::~Promise() = default;

v8::Maybe<bool> Promise::RejectWithErrorMessage(const std::string& string) {
  v8::HandleScope handle_scope(isolate());
  v8::MicrotasksScope script_scope(isolate(),
                                   v8::MicrotasksScope::kRunMicrotasks);
  v8::Context::Scope context_scope(
      v8::Local<v8::Context>::New(isolate(), GetContext()));

  v8::Local<v8::String> error_message =
      v8::String::NewFromUtf8(isolate(), string.c_str(),
                              v8::NewStringType::kNormal,
                              static_cast<int>(string.size()))
          .ToLocalChecked();
  v8::Local<v8::Value> error = v8::Exception::Error(error_message);
  return Reject(error);
}

v8::Local<v8::Promise> Promise::GetHandle() const {
  return GetInner()->GetPromise();
}

}  // namespace util

}  // namespace atom

namespace mate {

v8::Local<v8::Value> mate::Converter<atom::util::Promise*>::ToV8(
    v8::Isolate*,
    atom::util::Promise* val) {
  return val->GetHandle();
}

}  // namespace mate
