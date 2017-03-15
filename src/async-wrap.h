// Copyright Joyent, Inc. and other Node contributors.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
// USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef SRC_ASYNC_WRAP_H_
#define SRC_ASYNC_WRAP_H_

#if defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#include "base-object.h"
#include "uv.h"
#include "v8.h"

#include <stdint.h>

namespace node {

#define NODE_ASYNC_ID_OFFSET 0xA1C

#define NODE_ASYNC_PROVIDER_TYPES(V)                                          \
  V(NONE)                                                                     \
  V(CONNECTION)                                                               \
  V(FSEVENTWRAP)                                                              \
  V(FSREQWRAP)                                                                \
  V(GETADDRINFOREQWRAP)                                                       \
  V(GETNAMEINFOREQWRAP)                                                       \
  V(HTTPPARSER)                                                               \
  V(JSSTREAM)                                                                 \
  V(PBKDF2REQUEST)                                                            \
  V(PIPECONNECTWRAP)                                                          \
  V(PIPEWRAP)                                                                 \
  V(PROCESSWRAP)                                                              \
  V(QUERYWRAP)                                                                \
  V(RANDOMBYTESREQUEST)                                                       \
  V(SENDWRAP)                                                                 \
  V(SHUTDOWNWRAP)                                                             \
  V(SIGNALWRAP)                                                               \
  V(STATWATCHER)                                                              \
  V(TCPWRAP)                                                                  \
  V(TCPCONNECTWRAP)                                                           \
  V(TIMERWRAP)                                                                \
  V(TLSWRAP)                                                                  \
  V(TTYWRAP)                                                                  \
  V(UDPWRAP)                                                                  \
  V(WRITEWRAP)                                                                \
  V(ZCTX)

class Environment;

class AsyncWrap : public BaseObject {
 public:
  enum ProviderType {
#define V(PROVIDER)                                                           \
    PROVIDER_ ## PROVIDER,
    NODE_ASYNC_PROVIDER_TYPES(V)
#undef V
    PROVIDERS_LENGTH,
  };

  AsyncWrap(Environment* env,
            v8::Local<v8::Object> object,
            ProviderType provider,
            AsyncWrap* parent = nullptr);

  virtual ~AsyncWrap();

  static void Initialize(v8::Local<v8::Object> target,
                         v8::Local<v8::Value> unused,
                         v8::Local<v8::Context> context);

  static void DestroyIdsCb(uv_idle_t* handle);

  inline ProviderType provider_type() const;

  inline double get_id() const;

  // Only call these within a valid HandleScope.
  v8::Local<v8::Value> MakeCallback(const v8::Local<v8::Function> cb,
                                     int argc,
                                     v8::Local<v8::Value>* argv);
  inline v8::Local<v8::Value> MakeCallback(const v8::Local<v8::String> symbol,
                                            int argc,
                                            v8::Local<v8::Value>* argv);
  inline v8::Local<v8::Value> MakeCallback(uint32_t index,
                                            int argc,
                                            v8::Local<v8::Value>* argv);

  virtual size_t self_size() const = 0;

 private:
  inline AsyncWrap();
  inline bool ran_init_callback() const;

  // When the async hooks init JS function is called from the constructor it is
  // expected the context object will receive a _asyncQueue object property
  // that will be used to call pre/post in MakeCallback.
  uint32_t bits_;
  const double id_;
};

void LoadAsyncWrapperInfo(Environment* env);

}  // namespace node

#endif  // defined(NODE_WANT_INTERNALS) && NODE_WANT_INTERNALS

#endif  // SRC_ASYNC_WRAP_H_
