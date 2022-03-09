
#include <napi.h>
#include <thread>
#include <chrono>
#include <set>
#include <functional>
#include <vector>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libavutil/imgutils.h>
};

#include "instance-data.h"
#include "util.h"
#include "media-info-worker.h"
#include "avduration.h"
#include "avformat-context.h"
#include "avchapter.h"
#include "avprogram.h"
#include "avstream.h"


namespace node_ffprobe {



Napi::Promise GetFileInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    if (!info[0].IsString()) {
        deferred.Reject(Napi::TypeError::New(env, Napi::String::New(env,"filePath parameter should be a string value")).Value());
        return deferred.Promise();
    }

    std::string filePath = info[0].As<Napi::String>();

    MediaInfoWorker* worker = new MediaInfoWorker(env, filePath, std::move(deferred));

    worker->Queue();

    return deferred.Promise();
}

const Napi::Object GetVersionsObject(const Napi::Env& env) {
    Napi::Object versions = Napi::Object::New(env);

    Napi::PropertyDescriptor avformat = Napi::PropertyDescriptor::Value("avformat", 
            Napi::String::New(env, AV_STRINGIFY(LIBAVFORMAT_VERSION)), napi_enumerable);

    Napi::PropertyDescriptor avcodec = Napi::PropertyDescriptor::Value("avcodec", 
            Napi::String::New(env, AV_STRINGIFY(LIBAVCODEC_VERSION)), napi_enumerable);

    Napi::PropertyDescriptor avutil = Napi::PropertyDescriptor::Value("avutil", 
            Napi::String::New(env, AV_STRINGIFY(LIBAVUTIL_VERSION)), napi_enumerable);

    versions.DefineProperties({avformat, avcodec, avutil});

    return versions;
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {

    exports.Set(Napi::String::New(env, "getFileInfo"), Napi::Function::New(env, GetFileInfo));
    exports.Set(Napi::String::New(env, "versions"), GetVersionsObject(env));
    exports.Set(Napi::String::New(env, "AvFormatContext"), AvFormatContext::Define(env));
    exports.Set(Napi::String::New(env, "AvDuration"), AvDuration::Define(env));
    exports.Set(Napi::String::New(env, "AvChapter"), AvChapter::Define(env));
    exports.Set(Napi::String::New(env, "AvProgram"), AvProgram::Define(env));

    AvStream::Define(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)

};
