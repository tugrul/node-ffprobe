

#ifndef NODE_FFPROBE_AVCHAPTER_H
#define NODE_FFPROBE_AVCHAPTER_H

#include <napi.h>

namespace node_ffprobe {

class AvChapter : public Napi::ObjectWrap<AvChapter> {
public:
    AvChapter(const Napi::CallbackInfo& info); 
    static Napi::Function Define(Napi::Env env);
    static Napi::FunctionReference constructor;
};


}

#endif