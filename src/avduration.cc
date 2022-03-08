
#include <sstream>
#include "avduration.h"

extern "C" {
    #include <libavutil/avutil.h>
}

namespace node_ffprobe {

Napi::FunctionReference AvDuration::constructor;

AvDuration::AvDuration(const Napi::CallbackInfo& info) : 
    Napi::ObjectWrap<AvDuration>(info) {

    if (!SetDuration(info)) {
        return;
    }

    info.This().As<Napi::Object>().DefineProperties({

        Napi::PropertyDescriptor::Value("hours",
            Napi::Number::New(Env(), hours), napi_enumerable),
        
        Napi::PropertyDescriptor::Value("minutes",
            Napi::Number::New(Env(), minutes), napi_enumerable),

        Napi::PropertyDescriptor::Value("seconds",
            Napi::Number::New(Env(), seconds), napi_enumerable),

        Napi::PropertyDescriptor::Value("microseconds",
            Napi::Number::New(Env(), microseconds), napi_enumerable)

    });

}

Napi::Function AvDuration::Define(Napi::Env env) {

    Napi::Function func = DefineClass(env, "AvDuration", {
        InstanceMethod<&AvDuration::GetDuration>("valueOf"),
        InstanceMethod<&AvDuration::ToString>("toString")
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
}

bool AvDuration::SetDuration(const Napi::CallbackInfo& info) {

    duration = info[0].As<Napi::Number>();

    seconds = duration / AV_TIME_BASE;
    microseconds = ((duration % AV_TIME_BASE) * 100) / AV_TIME_BASE;
    minutes = seconds / 60;
    seconds %= 60;
    hours = minutes / 60;
    minutes %= 60;

    return true;
}

Napi::Value AvDuration::GetDuration(const Napi::CallbackInfo& info) {
    return Napi::Number::New(Env(), duration);
}

Napi::Value AvDuration::ToString(const Napi::CallbackInfo& info) {
    std::stringstream ss;

    if (hours < 10) {
        ss << "0";
    }

    ss << hours << ":";

    if (minutes < 10) {
        ss << "0";
    }

    ss << minutes << ":";

    if (seconds < 10) {
        ss << "0";
    }

    ss << seconds << ".";

    if (microseconds < 10) {
        ss << "0";
    }

    ss << microseconds;

    return Napi::String::New(Env(), ss.str());
}

};