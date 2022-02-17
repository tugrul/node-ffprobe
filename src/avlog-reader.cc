
#include "avlog-reader.h"

namespace node_ffprobe {

Napi::FunctionReference AvLogReader::constructor;

AvLogReader::AvLogReader(const Napi::CallbackInfo& info) : 
Napi::ObjectWrap<AvLogReader>(info) {

}

Napi::Value AvLogReader::Next(const Napi::CallbackInfo& info) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    return deferred.Promise();
}

Napi::Value AvLogReader::Return(const Napi::CallbackInfo& info) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    return deferred.Promise();
}

Napi::Value AvLogReader::Throw(const Napi::CallbackInfo& info) {
    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    return deferred.Promise();
}

Napi::Function AvLogReader::Define(Napi::Env env) {
    
    Napi::Function func = DefineClass(env, "AvLogReader", {
        InstanceMethod<&AvLogReader::Next>("next"), 
        InstanceMethod<&AvLogReader::Return>("return"),
        InstanceMethod<&AvLogReader::Throw>("throw")
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
}

};