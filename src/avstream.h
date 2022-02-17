

#ifndef NODE_FFPROBE_AVSTREAM_H
#define NODE_FFPROBE_AVSTREAM_H

#include <napi.h>

#include "avduration.h"
#include "util.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

namespace node_ffprobe {

typedef struct AvStreamContext {
    const AVInputFormat* format;
    AVStream* context;
} AvStreamContext;

class AvStream : public Napi::ObjectWrap<AvStream> {
public:
    AvStream(const Napi::CallbackInfo& info);
    static void Define(Napi::Env env, Napi::Object exports);
    static Napi::FunctionReference consVideo;
    static Napi::FunctionReference consAudio;
    static Napi::FunctionReference consData;
    static Napi::FunctionReference consSubtitle;
    static Napi::FunctionReference consAttachment;
private:
    void ExportCommonInfo(Napi::Object that);
    void ExportVideoInfo(Napi::Object that);
    void ExportAudioInfo(Napi::Object that);
    void ExportDataInfo(Napi::Object that);
    void ExportSubtitleInfo(Napi::Object that);
    void ExportAttachmentInfo(Napi::Object that);
    Napi::Value GetCodecProfileName();
    Napi::Value GetCodecTagName();
    Napi::Value GetVideoPixelFormat();
    Napi::Value GetVideoBpc();
    Napi::Value GetVideoColorRange();
    Napi::Value GetVideoColorSpace();
    Napi::Value GetVideoColorPrimaries();
    Napi::Value GetVideoColorTransfer();
    Napi::Value GetVideoChromaLocation();
    Napi::Value GetVideoFps();
    Napi::Value GetVideoTbr();
    Napi::Value GetVideoTbn();
    Napi::Array GetVideoDar();
    Napi::Array GetVideoSar();
    Napi::Value GetDataTbn();
    Napi::Value GetAudioChannelLayout();
    Napi::Value GetAudioSampleFormatName();
    Napi::Value GetAudioBitsPerRawSample();
    Napi::Object GetDisposition();
    int64_t GetBitRate();
    AVStream* context;
    const AVInputFormat* format;
};


}

#endif