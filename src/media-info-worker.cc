
#include "media-info-worker.h"


namespace node_ffprobe {

AVFormatContext* MediaInfoWorker::sharedContext = nullptr;
std::mutex MediaInfoWorker::sharedContextMutex;

MediaInfoWorker::MediaInfoWorker(const Napi::Env& env, const std::string& fileName, const uint64_t probeSize, const uint64_t analyzeDuration, Napi::Promise::Deferred&& def): 
        Napi::AsyncWorker(env), 
        fileName(fileName), 
	probeSize(probeSize),
        analyzeDuration(analyzeDuration),
        deferred(std::move(def)) {}

void MediaInfoWorker::Execute() {

    avFormatContext = avformat_alloc_context();

    if (!avFormatContext) {
        Napi::AsyncWorker::SetError("aformat context memory could not be allocated");
        return;
    }

    AVDictionary* options = nullptr;

    if (probeSize) {
        av_dict_set_int(&options, "probesize", probeSize, 0);
    }

    if (analyzeDuration) {
        av_dict_set_int(&options, "analyzeduration", analyzeDuration, 0);
    }

    int openStatus = avformat_open_input(&avFormatContext, fileName.c_str(), NULL, &options);

    if (options) {
        av_dict_free(&options);
    }

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
