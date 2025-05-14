
#include "media-info-worker.h"


namespace node_ffprobe {

AVFormatContext* MediaInfoWorker::sharedContext = nullptr;
std::mutex MediaInfoWorker::sharedContextMutex;

MediaInfoWorker::MediaInfoWorker(const Napi::Env& env, const std::string& fileName, AvDictionary&& options, Napi::Promise::Deferred&& def): 
        Napi::AsyncWorker(env), 
        fileName(fileName),
        options(std::move(options)),
        deferred(std::move(def)) {}

void MediaInfoWorker::Execute() {

    avFormatContext = avformat_alloc_context();

    if (!avFormatContext) {
        Napi::AsyncWorker::SetError("aformat context memory could not be allocated");
        return;
    }

    int openStatus = avformat_open_input(&avFormatContext, fileName.c_str(), NULL, options);

    if (openStatus < 0) {
        Napi::AsyncWorker::SetError("filePath could not be opened");
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


};
