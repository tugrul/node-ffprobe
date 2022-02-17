
#ifndef NODE_FFPROBE_AVFORMAT_CONTEXT_H
#define NODE_FFPROBE_AVFORMAT_CONTEXT_H

#include <napi.h>
#include <mutex>
#include "avstream.h"

extern "C" {
    #include <libavformat/avformat.h>
}

namespace node_ffprobe {

class AvFormatContext : public Napi::ObjectWrap<AvFormatContext> {
public:
    AvFormatContext(const Napi::CallbackInfo& info);    
    ~AvFormatContext();
    static Napi::Function Define(Napi::Env env, AVFormatContext* data = nullptr);
    static Napi::FunctionReference constructor;
    static AvStreamContext* sharedContext;
private:
    AVFormatContext* context;
    static std::mutex sharedContextMutex;
    void ExportCommonInfo(Napi::Object that);
    Napi::Array ExportStreamInfo(Napi::Object that);
    void ExportChapterInfo(Napi::Object that);
    void ExportProgramInfo(Napi::Object that, Napi::Array streams);
};

};

#endif
