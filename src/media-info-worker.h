
#ifndef NODE_FFPROBE_MEDIA_INFO_WORKER_H
#define NODE_FFPROBE_MEDIA_INFO_WORKER_H

#include <napi.h>
#include <mutex>

extern "C" {
    #include <libavformat/avformat.h>
}

#include "avformat-context.h"
#include "avdictionary.h"

namespace node_ffprobe {


class MediaInfoWorker : public Napi::AsyncWorker {
public:
    MediaInfoWorker(const Napi::Env& env, const std::string& fileName, AvDictionary&& options, Napi::Promise::Deferred&& def);
    void Execute() override;
    void OnOK() override;
    void OnError(const Napi::Error& error) override;
    Napi::Promise GetPromise();

    static AVFormatContext* sharedContext;

private:
    std::string fileName;
    AvDictionary options;
    Napi::Promise::Deferred deferred;
    static std::mutex sharedContextMutex;
    AVFormatContext* avFormatContext = nullptr;
};

};



#endif


