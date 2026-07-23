
#include "media-info-worker.h"
#include "shared-slot.h"


namespace node_ffprobe {

namespace {

/** Appends FFmpeg's own description of an AVERROR code to a message. */
std::string DescribeAvError(const char* message, int code) {

    char buf[AV_ERROR_MAX_STRING_SIZE];

    if (av_strerror(code, buf, sizeof(buf)) < 0) {
        return std::string(message);
    }

    buf[sizeof(buf) - 1] = '\0';

    return std::string(message) + ": " + buf;
}

};

AVFormatContext* MediaInfoWorker::sharedContext = nullptr;
std::mutex MediaInfoWorker::sharedContextMutex;

MediaInfoWorker::MediaInfoWorker(const Napi::Env& env, const std::string& fileName, AvDictionary&& options, Napi::Promise::Deferred&& def): 
        Napi::AsyncWorker(env), 
        fileName(fileName),
        options(std::move(options)),
        deferred(std::move(def)) {}

MediaInfoWorker::~MediaInfoWorker() {
    // Only reached while this worker still owns the context: ownership is
    // released in OnOK() once the JS wrapper has taken it over.
    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
    }
}

void MediaInfoWorker::Execute() {

    avFormatContext = avformat_alloc_context();

    if (!avFormatContext) {
        errorCode = AVERROR(ENOMEM);
        Napi::AsyncWorker::SetError("avformat context memory could not be allocated");
        return;
    }

    int openStatus = avformat_open_input(&avFormatContext, fileName.c_str(), NULL, options);

    if (openStatus < 0) {
        errorCode = openStatus;
        Napi::AsyncWorker::SetError(DescribeAvError("filePath could not be opened", openStatus));
        return;
    }

    const int infoStatus = avformat_find_stream_info(avFormatContext, NULL);

    if (infoStatus < 0) {
        errorCode = infoStatus;
        Napi::AsyncWorker::SetError(DescribeAvError("could not get stream info", infoStatus));
        return;
    }
}


void MediaInfoWorker::OnOK() {
    Napi::HandleScope scope(Env());

    std::lock_guard<std::mutex> guard(sharedContextMutex);

    // Cleared again even if the wrapper's constructor throws.
    SharedSlot<AVFormatContext> slot(&sharedContext, avFormatContext);

    Napi::Object result = AvFormatContext::constructor.New({});

    // The wrapper owns the context from here on; it closes it on destruction.
    avFormatContext = nullptr;

    deferred.Resolve(result);
}


void MediaInfoWorker::OnError(const Napi::Error& error) {

    Napi::Object value = error.Value();

    if (errorCode != 0) {
        value.Set("code", Napi::Number::New(Env(), errorCode));
    }

    deferred.Reject(value);
}


};
