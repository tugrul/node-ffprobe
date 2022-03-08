
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
#include "avlog-reader.h"
#include "avstream.h"


namespace node_ffprobe {

typedef std::set<std::reference_wrapper<AvLogReader>, std::less<AvLogReader>> LogReaderInstanceSet;

std::mutex logMutex;
static int printLogPrefix = 1;

static LogReaderInstanceSet logReaders;

// static std::vector<Napi::ObjectReference> objects;

static void LogCallback(void* ptr, int level, const char* fmt, va_list vl) {

    std::lock_guard<std::mutex> guard(logMutex);

    if (logReaders.empty()) {
        return;
    }

    char line[256];

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &printLogPrefix);

    for (AvLogReader& reader: logReaders) {
        reader.Push(level, line);
    }

}


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

Napi::Value GetLogReader(const Napi::CallbackInfo& info) {
    
    std::lock_guard<std::mutex> guard(logMutex);

    Napi::Object result = AvLogReader::constructor.New({info[0]});

    //objects.push_back(Napi::Persistent(result));

    logReaders.insert(std::ref(*AvLogReader::Unwrap(result)));

    return result;
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


Napi::Promise TestProm(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    uint8_t number = 0;

    std::thread runner([&number] {

        std::this_thread::sleep_for(std::chrono::seconds(2));

        number = 10;

    });

    runner.join();

    deferred.Resolve(Napi::Number::New(env, number));

    return deferred.Promise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {

    av_log_set_callback(&LogCallback);

    exports.Set(Napi::String::New(env, "getProm"), Napi::Function::New(env, TestProm));
    exports.Set(Napi::String::New(env, "getFileInfo"), Napi::Function::New(env, GetFileInfo));
    exports.Set(Napi::String::New(env, "getLogReader"), Napi::Function::New(env, GetLogReader));
    exports.Set(Napi::String::New(env, "versions"), GetVersionsObject(env));
    exports.Set(Napi::String::New(env, "AvFormatContext"), AvFormatContext::Define(env));
    exports.Set(Napi::String::New(env, "AvDuration"), AvDuration::Define(env));
    exports.Set(Napi::String::New(env, "AvChapter"), AvChapter::Define(env));
    exports.Set(Napi::String::New(env, "AvProgram"), AvProgram::Define(env));
    exports.Set(Napi::String::New(env, "AvLogReader"), AvLogReader::Define(env));

    AvStream::Define(env, exports);

    return exports;
}

NODE_API_MODULE(addon, Init)

};
