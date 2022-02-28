
#include "media-info-worker.h"


namespace node_ffprobe {

AVFormatContext* MediaInfoWorker::sharedContext = nullptr;
std::mutex MediaInfoWorker::sharedContextMutex;

MediaInfoWorker::MediaInfoWorker(const Napi::Env& env, const std::string& fileName): 
        Napi::AsyncWorker(env), 
        fileName(fileName), 
        deferred(Napi::Promise::Deferred::New(env)) {}

void MediaInfoWorker::Execute() {

    avFormatContext = avformat_alloc_context();

    if (!avFormatContext) {
        Napi::AsyncWorker::SetError("aformat context memory could not be allocated");
        return;
    }

    int openStatus = avformat_open_input(&avFormatContext, fileName.c_str(), NULL, NULL);

    if (openStatus < 0) {
        Napi::AsyncWorker::SetError("file could not be opened");
        return;
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        Napi::AsyncWorker::SetError("could not get stream info");
    }
}


void MediaInfoWorker::OnOK() {
    Napi::HandleScope scope(Env());

    std::lock_guard<std::mutex> guard(sharedContextMutex);

    sharedContext = avFormatContext;

    Napi::Object result = AvFormatContext::constructor.New({});

    sharedContext = nullptr;

    deferred.Resolve(result);
}


void MediaInfoWorker::OnError(const Napi::Error& error) {
    deferred.Reject(error.Value());
}


Napi::Promise MediaInfoWorker::GetPromise() {
    return deferred.Promise();
}

};