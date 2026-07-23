
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
#include "avdictionary.h"


namespace node_ffprobe {



Napi::Promise GetFileInfo(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);


    if (!info[0].IsString()) {
        deferred.Reject(Napi::TypeError::New(env, Napi::String::New(env,"filePath parameter should be a string value")).Value());
        return deferred.Promise();
    }

    AvDictionary options;

    if (!info[1].IsUndefined()) {
        if (!info[1].IsObject()) {
            deferred.Reject(Napi::TypeError::New(env, Napi::String::New(env,"options can only be an object")).Value());
            return deferred.Promise();
        }

        Napi::Object opts = info[1].As<Napi::Object>();

        if (opts.Has("probeSize") && opts.Get("probeSize").IsNumber()) {
            options.set("probesize", opts.Get("probeSize").As<Napi::Number>().Int64Value());
        }

        if (opts.Has("analyzeDuration") && opts.Get("analyzeDuration").IsNumber()) {
            options.set("analyzeduration", opts.Get("analyzeDuration").As<Napi::Number>().Int64Value());
        }

        if (opts.Has("protocolWhitelist") && !opts.Get("protocolWhitelist").IsUndefined()) {

            Napi::Value whitelist = opts.Get("protocolWhitelist");
            std::string value;

            if (whitelist.IsString()) {
                value = whitelist.As<Napi::String>().Utf8Value();
            } else if (whitelist.IsArray()) {

                Napi::Array entries = whitelist.As<Napi::Array>();

                for (uint32_t index = 0; index < entries.Length(); index++) {

                    Napi::Value entry = entries.Get(index);

                    if (!entry.IsString()) {
                        deferred.Reject(Napi::TypeError::New(env, Napi::String::New(env,
                            "protocolWhitelist entries should be string values")).Value());
                        return deferred.Promise();
                    }

                    if (index > 0) {
                        value += ",";
                    }

                    value += entry.As<Napi::String>().Utf8Value();
                }

            } else {
                deferred.Reject(Napi::TypeError::New(env, Napi::String::New(env,
                    "protocolWhitelist can only be a string or an array of strings")).Value());
                return deferred.Promise();
            }

            options.set("protocol_whitelist", value);
        }
    }

    std::string filePath = info[0].As<Napi::String>();

    MediaInfoWorker* worker = new MediaInfoWorker(env, filePath, std::move(options), std::move(deferred));

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

    // Owned by this environment and destroyed with it. Init() runs once per
    // environment, so a worker_thread gets its own constructors rather than
    // overwriting the ones belonging to the main thread.
    InstanceData* instanceData = new InstanceData();

    env.SetInstanceData(instanceData);

    exports.Set(Napi::String::New(env, "getFileInfo"), Napi::Function::New(env, GetFileInfo));
    exports.Set(Napi::String::New(env, "versions"), GetVersionsObject(env));

    Napi::Function avFormatContext = AvFormatContext::Define(env, instanceData);
    instanceData->avFormatContext = Napi::Persistent(avFormatContext);
    exports.Set(Napi::String::New(env, "AvFormatContext"), avFormatContext);

    Napi::Function avDuration = AvDuration::Define(env);
    instanceData->avDuration = Napi::Persistent(avDuration);
    exports.Set(Napi::String::New(env, "AvDuration"), avDuration);

    Napi::Function avChapter = AvChapter::Define(env);
    instanceData->avChapter = Napi::Persistent(avChapter);
    exports.Set(Napi::String::New(env, "AvChapter"), avChapter);

    Napi::Function avProgram = AvProgram::Define(env);
    instanceData->avProgram = Napi::Persistent(avProgram);
    exports.Set(Napi::String::New(env, "AvProgram"), avProgram);

    AvStream::Define(env, exports, instanceData);

    return exports;
}

NODE_API_MODULE(addon, Init)

};
