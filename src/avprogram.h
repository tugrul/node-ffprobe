

#ifndef NODE_FFPROBE_AVPROGRAM_H
#define NODE_FFPROBE_AVPROGRAM_H

#include <napi.h>

namespace node_ffprobe {

class AvProgram : public Napi::ObjectWrap<AvProgram> {
public:
    AvProgram(const Napi::CallbackInfo& info); 
    static Napi::Function Define(Napi::Env env);
    static Napi::FunctionReference constructor;
};


}

#endif