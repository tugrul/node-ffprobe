
#include "avprogram.h"

namespace node_ffprobe {

Napi::FunctionReference AvProgram::constructor;

AvProgram::AvProgram(const Napi::CallbackInfo& info) : 
    Napi::ObjectWrap<AvProgram>(info)  {

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(Env(), "Program ID value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(Env(), "Flags value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (!info[2].IsNumber()) {
        Napi::TypeError::New(Env(), "Discard value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (!info[3].IsArray()) {
        Napi::TypeError::New(Env(), "Streams value should be an array").ThrowAsJavaScriptException();
        return;
    }

    Napi::Object that = info.This().As<Napi::Object>();

    that.DefineProperties({
        Napi::PropertyDescriptor::Value("id",
            info[0].As<Napi::Number>(), napi_enumerable), 
        Napi::PropertyDescriptor::Value("flags",
            info[1].As<Napi::Number>(), napi_enumerable), 
        Napi::PropertyDescriptor::Value("discard",
            info[2].As<Napi::Number>(), napi_enumerable),
        Napi::PropertyDescriptor::Value("streams",
            info[3].As<Napi::Array>(), napi_enumerable)
    });

    if (info[4].IsObject() && !info[4].IsArray()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("metadata",
            info[4].As<Napi::Object>(), napi_enumerable));
    } else if (!info[4].IsUndefined()) {
        Napi::TypeError::New(Env(), "Metadata value should be object").ThrowAsJavaScriptException();
        return;
    }

    if (info[5].IsNumber()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("programNum",
            info[5].As<Napi::Number>(), napi_enumerable));
    } else if (!info[5].IsUndefined()) {
        Napi::TypeError::New(Env(), "ProgramNum value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (info[6].IsNumber()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("pmtPid",
            info[6].As<Napi::Number>(), napi_enumerable));
    } else if (!info[6].IsUndefined()) {
        Napi::TypeError::New(Env(), "PmtPid value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (info[7].IsNumber()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("pcrPid",
            info[7].As<Napi::Number>(), napi_enumerable));
    } else if (!info[7].IsUndefined()) {
        Napi::TypeError::New(Env(), "PcrPid value should be numeric").ThrowAsJavaScriptException();
        return;
    }

    if (info[8].IsNumber()) {
        that.DefineProperty(Napi::PropertyDescriptor::Value("pmtVersion",
            info[8].As<Napi::Number>(), napi_enumerable));
    } else if (!info[8].IsUndefined()) {
        Napi::TypeError::New(Env(), "PmtVersion value should be numeric").ThrowAsJavaScriptException();
        return;
    }
}


Napi::Function AvProgram::Define(Napi::Env env) {
    
    Napi::Function func = DefineClass(env, "AvProgram", {});

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
}

};