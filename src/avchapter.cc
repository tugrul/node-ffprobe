
#include "avchapter.h"

namespace node_ffprobe {

Napi::FunctionReference AvChapter::constructor;

AvChapter::AvChapter(const Napi::CallbackInfo& info) : 
    Napi::ObjectWrap<AvChapter>(info)  {

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(Env(), "Chapter ID value should be numberic").ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(Env(), "Chapter start time should be numberic").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsNumber()) {
        Napi::TypeError::New(Env(), "Chapter end time should be numberic").ThrowAsJavaScriptException();
        return;
    }

    Napi::Object that = info.This().As<Napi::Object>();

    that.DefineProperties({
        Napi::PropertyDescriptor::Value("id",
            info[0].As<Napi::Number>(), napi_enumerable), 
        Napi::PropertyDescriptor::Value("start",
            info[1].As<Napi::Number>(), napi_enumerable), 
        Napi::PropertyDescriptor::Value("end",
            info[2].As<Napi::Number>(), napi_enumerable)
    });

    if (info[3].IsObject()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("metadata",
            info[3].As<Napi::Object>(), napi_enumerable));
    }
}

Napi::Function AvChapter::Define(Napi::Env env) {
    
    Napi::Function func = DefineClass(env, "AvChapter", {});

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
}

};