
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

    std::string filePath;
    int64_t probeSize = 0;
    int64_t analyzeDuration = 0;

    if (info[0].IsString()) {

        filePath = info[0].As<Napi::String>();
    }
    else if (info[0].IsObject()) {

        Napi::Object obj = info[0].As<Napi::Object>();

        if (!obj.Has("filePath") || !obj.Get("filePath").IsString()) {
            deferred.Reject(Napi::TypeError::New(env, "Missing or invalid 'filePath' field").Value());
            return deferred.Promise();
        }

        filePath = obj.Get("filePath").As<Napi::String>();

        if (obj.Has("options") && obj.Get("options").IsObject()) {

            Napi::Object opts = obj.Get("options").As<Napi::Object>();

            if (opts.Has("probeSize") && opts.Get("probeSize").IsNumber()) {
                probeSize = opts.Get("probeSize").As<Napi::Number>().Int64Value();
            }

            if (opts.Has("analyzeDuration") && opts.Get("analyzeDuration").IsNumber()) {
                analyzeDuration = opts.Get("analyzeDuration").As<Napi::Number>().Int64Value();
            }
        }
    }
    else {

        deferred.Reject(Napi::TypeError::New(env, "Expected string or object as first argument").Value());
        return deferred.Promise();
    }


    MediaInfoWorker* worker = new MediaInfoWorker(env, filePath, probeSize, analyzeDuration, std::move(deferred));

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
