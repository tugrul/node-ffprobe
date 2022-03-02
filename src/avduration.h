

#ifndef NODE_FFPROBE_AVDURATION_H
#define NODE_FFPROBE_AVDURATION_H

#include <napi.h>

namespace node_ffprobe {

class AvDuration: public Napi::ObjectWrap<AvDuration> {
public:
    AvDuration(const Napi::CallbackInfo& info);
    static Napi::Function Define(Napi::Env env);
    static Napi::FunctionReference constructor;
private:
    Napi::Value GetDuration(const Napi::CallbackInfo& info);
    bool SetDuration(const Napi::CallbackInfo& info);
    Napi::Value ToString(const Napi::CallbackInfo& info);

    int64_t duration;
    int64_t hours;
    int64_t minutes;
    int64_t seconds;
    int64_t microseconds;
};

};


#endif
