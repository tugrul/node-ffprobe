
#include "avstream.h"
#include "avformat-context.h"

extern "C" {
#include <libavutil/pixdesc.h>
#include <libavutil/channel_layout.h>
}

namespace node_ffprobe {

Napi::FunctionReference AvStream::consVideo;
Napi::FunctionReference AvStream::consAudio;
Napi::FunctionReference AvStream::consData;
Napi::FunctionReference AvStream::consSubtitle;
Napi::FunctionReference AvStream::consAttachment;

AvStream::AvStream(const Napi::CallbackInfo& info) : 
    Napi::ObjectWrap<AvStream>(info) {

    if (info.Data() == nullptr) {
        Napi::Error::New(Env(), "CallbackInfo::Data should have AVStream").ThrowAsJavaScriptException();
        return;
    }

    AvStreamContext* data = *static_cast<AvStreamContext**>(info.Data());

    context = data->context;
    format = data->format;

    if (context == nullptr) {
        Napi::Error::New(Env(), "Direct initialization is not allowed for this wrapper class").ThrowAsJavaScriptException();
        return;
    }

    Napi::Object that = info.This().As<Napi::Object>();
    ExportCommonInfo(that);

    if (context->codecpar != nullptr) {
        switch (context->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO: ExportVideoInfo(that); break;
            case AVMEDIA_TYPE_AUDIO: ExportAudioInfo(that); break;
            case AVMEDIA_TYPE_UNKNOWN:
            case AVMEDIA_TYPE_DATA: ExportDataInfo(that); break;
            case AVMEDIA_TYPE_SUBTITLE: ExportSubtitleInfo(that); break;
            case AVMEDIA_TYPE_ATTACHMENT: ExportAttachmentInfo(that); break; 
            default:;
        }
    }
    
}

void AvStream::Define(Napi::Env env, Napi::Object exports) {

    Napi::Function videoFunc = DefineClass(env, "AvStreamVideo", {}, &AvFormatContext::sharedContext);

    consVideo = Napi::Persistent(videoFunc);
    consVideo.SuppressDestruct();

    exports.Set(Napi::String::New(env, "AvStreamVideo"), videoFunc);


    Napi::Function audioFunc = DefineClass(env, "AvStreamAudio", {}, &AvFormatContext::sharedContext);

    consAudio = Napi::Persistent(audioFunc);
    consAudio.SuppressDestruct();

    exports.Set(Napi::String::New(env, "AvStreamAudio"), audioFunc);

    Napi::Function dataFunc = DefineClass(env, "AvStreamData", {}, &AvFormatContext::sharedContext);

    consData = Napi::Persistent(dataFunc);
    consData.SuppressDestruct();

    exports.Set(Napi::String::New(env, "AvStreamData"), dataFunc);

    Napi::Function subtitleFunc = DefineClass(env, "AvStreamSubtitle", {}, &AvFormatContext::sharedContext);

    consSubtitle = Napi::Persistent(subtitleFunc);
    consSubtitle.SuppressDestruct();

    exports.Set(Napi::String::New(env, "AvStreamSubtitle"), subtitleFunc);

    Napi::Function attachmentFunc = DefineClass(env, "AvStreamAttachment", {}, &AvFormatContext::sharedContext);

    consAttachment = Napi::Persistent(attachmentFunc);
    consAttachment.SuppressDestruct();

    exports.Set(Napi::String::New(env, "AvStreamAttachment"), attachmentFunc);
}


int64_t AvStream::GetBitRate() {

    AVCodecParameters* codecpar = context->codecpar;

    int bitsPerSample = 0;
    int64_t bitRate = 0;

    switch (codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
        case AVMEDIA_TYPE_DATA:
        case AVMEDIA_TYPE_SUBTITLE:
        case AVMEDIA_TYPE_ATTACHMENT:
            bitRate = codecpar->bit_rate;
            break;
        case AVMEDIA_TYPE_AUDIO:
            bitsPerSample = av_get_bits_per_sample(codecpar->codec_id);

            if (bitsPerSample) {
                bitRate = codecpar->sample_rate * (int64_t)codecpar->channels;
                if (bitRate > INT64_MAX / bitsPerSample) {
                    bitRate = 0;
                } else {
                    bitRate *= bitsPerSample;
                }

            } else {
                bitRate = codecpar->bit_rate;
            }

            break;
        default:;
    }

    return bitRate;
}

Napi::Value AvStream::GetVideoPixelFormat() {

    AVPixelFormat pixFmt = (AVPixelFormat)context->codecpar->format;

    if (pixFmt == AV_PIX_FMT_NONE) {
        return Env().Null();
    }

    const char* pixFmtName = av_get_pix_fmt_name(pixFmt);

    if (pixFmtName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), pixFmtName);
}

Napi::Value AvStream::GetVideoBpc() {

    if (!context->codecpar->bits_per_raw_sample) {
        return Env().Null();
    }

    AVPixelFormat pixFmt = (AVPixelFormat)context->codecpar->format;

    if (pixFmt == AV_PIX_FMT_NONE) {
        return Env().Null();
    }

    const AVPixFmtDescriptor* pixFmtDesc = av_pix_fmt_desc_get(pixFmt);

    if (context->codecpar->bits_per_raw_sample < pixFmtDesc->comp[0].depth) {
        return Napi::Number::New(Env(), context->codecpar->bits_per_raw_sample);
    }

    return Env().Null();
}

Napi::Value AvStream::GetVideoColorRange() {

    AVColorRange colorRange = context->codecpar->color_range;

    if (colorRange == AVCOL_RANGE_UNSPECIFIED) {
        return Env().Null();
    }

    const char* colorRangeName = av_color_range_name(colorRange);

    if (colorRangeName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), colorRangeName);
}

Napi::Value AvStream::GetVideoColorSpace() {

    AVColorSpace colorSpace = context->codecpar->color_space;

    if (colorSpace == AVCOL_SPC_UNSPECIFIED) {
        return Env().Null();
    }

    const char* colorSpaceName = av_color_space_name(colorSpace);

    if (colorSpaceName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), colorSpaceName);
}

Napi::Value AvStream::GetVideoColorPrimaries() {

    AVColorPrimaries colorPrimaries = context->codecpar->color_primaries;

    if (colorPrimaries == AVCOL_PRI_UNSPECIFIED) {
        return Env().Null();
    }

    const char* colorPrimariesName = av_color_primaries_name(colorPrimaries);

    if (colorPrimariesName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), colorPrimariesName);

}

Napi::Value AvStream::GetCodecTagName() {

    if (context->codecpar->codec_tag == 0) {
        return Env().Null();
    }

    FourCCIdentifier codecTag = {.codecTag = context->codecpar->codec_tag};

    return Napi::String::New(Env(), codecTag.name, 4);
}

Napi::Value AvStream::GetVideoColorTransfer() {

    AVColorTransferCharacteristic colorTransfer = context->codecpar->color_trc;

    if (colorTransfer == AVCOL_TRC_UNSPECIFIED) {
        return Env().Null();
    }

    const char* colorTransferName = av_color_transfer_name(colorTransfer);

    if (colorTransferName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), colorTransferName);
}

Napi::Value AvStream::GetVideoChromaLocation() {

    AVChromaLocation chromaLocation = context->codecpar->chroma_location;

    if (chromaLocation == AVCHROMA_LOC_UNSPECIFIED) {
        return Env().Null();
    }

    const char* chromaLocationName = av_chroma_location_name(chromaLocation);

    if (chromaLocationName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), chromaLocationName);
}

Napi::Array AvStream::GetVideoDar() {
    const AVCodecParameters* codecpar = context->codecpar;
    AVRational displayAspectRatio;

    av_reduce(&displayAspectRatio.num, &displayAspectRatio.den, 
        codecpar->width * (int64_t)codecpar->sample_aspect_ratio.num,
        codecpar->height * (int64_t)codecpar->sample_aspect_ratio.den,
        1024 * 1024);

    Napi::Array dar = Napi::Array::New(Env(), 2);
    
    dar["0"] = Napi::Number::New(Env(), displayAspectRatio.num);
    dar["1"] = Napi::Number::New(Env(), displayAspectRatio.den);

    return dar;
}

Napi::Array AvStream::GetVideoSar() {
    Napi::Array sar = Napi::Array::New(Env(), 2);
    
    sar["0"] = Napi::Number::New(Env(), context->codecpar->sample_aspect_ratio.num);
    sar["1"] = Napi::Number::New(Env(), context->codecpar->sample_aspect_ratio.den);

    return sar;
}

Napi::Value AvStream::GetCodecProfileName() {

    const char* profileName = avcodec_profile_name(context->codecpar->codec_id, context->codecpar->profile);

    if (profileName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), profileName);
}

Napi::Value AvStream::GetAudioChannelLayout() {
    char buf[256];

    av_get_channel_layout_string(buf, sizeof(buf), context->codecpar->channels, context->codecpar->channel_layout);

    return Napi::String::New(Env(), buf);
}

Napi::Value AvStream::GetAudioSampleFormatName() {

    if (context->codecpar->format == AV_SAMPLE_FMT_NONE) {
        return Env().Null();
    }

    const char* formatName = av_get_sample_fmt_name((AVSampleFormat)context->codecpar->format);

    if (formatName == nullptr) {
        return Env().Null();
    }

    return Napi::String::New(Env(), formatName);
}

Napi::Value AvStream::GetAudioBitsPerRawSample() {

    const AVCodecParameters* codecpar = context->codecpar;

    if (codecpar->bits_per_raw_sample <= 0) {
        return Env().Null();
    }

    if (codecpar->bits_per_raw_sample != av_get_bytes_per_sample((AVSampleFormat)codecpar->format) * 8) {
        return Napi::Number::New(Env(), codecpar->bits_per_raw_sample);
    }

    return Env().Null();

}

void AvStream::ExportDataInfo(Napi::Object that) {
    that.Set(Napi::String::New(Env(), "tbn"), GetDataTbn());
}

void AvStream::ExportSubtitleInfo(Napi::Object that) {

    const AVCodecParameters* codecpar = context->codecpar;

    that.Set(Napi::String::New(Env(), "width"),
        Napi::Number::New(Env(), (int)codecpar->width));

    that.Set(Napi::String::New(Env(), "height"),
        Napi::Number::New(Env(), (int)codecpar->height));
}

void AvStream::ExportAttachmentInfo(Napi::Object that) {
}

Napi::Value AvStream::GetVideoFps() {

    if (context->avg_frame_rate.den && context->avg_frame_rate.num) {
        return Napi::Number::New(Env(), av_q2d(context->avg_frame_rate));
    }

    return Env().Null();
}

Napi::Value AvStream::GetVideoTbr() {

    if (context->r_frame_rate.den && context->r_frame_rate.num) {
        return Napi::Number::New(Env(), av_q2d(context->r_frame_rate));
    }

    return Env().Null();
}

Napi::Value AvStream::GetVideoTbn() {

    if (context->time_base.den && context->time_base.num) {
        return Napi::Number::New(Env(), 1 / av_q2d(context->time_base));
    }

    return Env().Null();
}

Napi::Value AvStream::GetDataTbn() {

    if (context->time_base.den && context->time_base.num) {
        const int g = av_gcd(context->time_base.num, context->time_base.den);

        Napi::Array parts = Napi::Array::New(Env(), 2);

        parts["0"] = context->time_base.num / g;
        parts["1"] = context->time_base.den / g;

        return parts;
    }

    return Env().Null();
}

Napi::Object AvStream::GetDisposition() {

    Napi::Object obj = Napi::Object::New(Env());

    const int disp = context->disposition;

    obj.DefineProperties({
        Napi::PropertyDescriptor::Value("default", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_DEFAULT), napi_enumerable),

        Napi::PropertyDescriptor::Value("dub", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_DUB), napi_enumerable),

        Napi::PropertyDescriptor::Value("original", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_ORIGINAL), napi_enumerable),

        Napi::PropertyDescriptor::Value("comment", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_COMMENT), napi_enumerable),

        Napi::PropertyDescriptor::Value("lyrics", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_LYRICS), napi_enumerable),

        Napi::PropertyDescriptor::Value("karaoke", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_KARAOKE), napi_enumerable),

        Napi::PropertyDescriptor::Value("forced", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_FORCED), napi_enumerable),

        Napi::PropertyDescriptor::Value("hearingImpaired", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_HEARING_IMPAIRED), napi_enumerable),

        Napi::PropertyDescriptor::Value("visualImpaired", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_VISUAL_IMPAIRED), napi_enumerable),

        Napi::PropertyDescriptor::Value("cleanEffects", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_CLEAN_EFFECTS), napi_enumerable),

        Napi::PropertyDescriptor::Value("attachedPic", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_ATTACHED_PIC), napi_enumerable),

        Napi::PropertyDescriptor::Value("timedThumbnails", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_TIMED_THUMBNAILS), napi_enumerable),

        Napi::PropertyDescriptor::Value("captions", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_CAPTIONS), napi_enumerable),

        Napi::PropertyDescriptor::Value("descriptions", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_DESCRIPTIONS), napi_enumerable),

        Napi::PropertyDescriptor::Value("metadata", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_METADATA), napi_enumerable),

        Napi::PropertyDescriptor::Value("dependent", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_DEPENDENT), napi_enumerable),

        Napi::PropertyDescriptor::Value("stillImage", 
            Napi::Boolean::New(Env(), disp & AV_DISPOSITION_STILL_IMAGE), napi_enumerable)
    });

    return obj;
}

void AvStream::ExportCommonInfo(Napi::Object that) {

    AVCodecParameters* codecpar = context->codecpar;

    int flags = format->flags;

    that.DefineProperties({
        Napi::PropertyDescriptor::Value("id",
            flags & AVFMT_SHOW_IDS ? Napi::Number::New(Env(), context->id) : Env().Null(), napi_enumerable),

        Napi::PropertyDescriptor::Value("duration",
            context->duration == AV_NOPTS_VALUE ? Env().Null() : 
            AvDuration::constructor.New({Napi::Number::New(Env(), context->duration)}), napi_enumerable),

        Napi::PropertyDescriptor::Value("startTime",
            context->start_time == AV_NOPTS_VALUE ? Env().Null() : 
            AvDuration::constructor.New({Napi::Number::New(Env(), context->start_time)}), napi_enumerable),

        Napi::PropertyDescriptor::Value("codecTag",
            GetCodecTagName(), napi_enumerable),

        Napi::PropertyDescriptor::Value("bitrate",
            Napi::Number::New(Env(), GetBitRate()), napi_enumerable),

        Napi::PropertyDescriptor::Value("profile",
            GetCodecProfileName(), napi_enumerable),

        Napi::PropertyDescriptor::Value("level",
            codecpar->level == FF_LEVEL_UNKNOWN ? Env().Null() : Napi::Number::New(Env(), (int)codecpar->level), napi_enumerable),

        Napi::PropertyDescriptor::Value("codecName",
            Napi::String::New(Env(), avcodec_get_name(codecpar->codec_id)), napi_enumerable),

        Napi::PropertyDescriptor::Value("disposition", 
            GetDisposition(), napi_enumerable),

        Napi::PropertyDescriptor::Value("metadata",
            getMetadata(Env(), context->metadata), napi_enumerable)
    });
}

void AvStream::ExportVideoInfo(Napi::Object that) {

    const AVCodecParameters* codecpar = context->codecpar;

    that.DefineProperties({
        Napi::PropertyDescriptor::Value("pixFmt", GetVideoPixelFormat(), napi_enumerable),
        Napi::PropertyDescriptor::Value("bpc", GetVideoBpc(), napi_enumerable),
        Napi::PropertyDescriptor::Value("colorRange", GetVideoColorRange(), napi_enumerable),
        Napi::PropertyDescriptor::Value("colorSpace", GetVideoColorSpace(), napi_enumerable),
        Napi::PropertyDescriptor::Value("colorPrimaries", GetVideoColorPrimaries(), napi_enumerable),
        Napi::PropertyDescriptor::Value("colorTransferCharacteristic", GetVideoColorTransfer(), napi_enumerable),
        Napi::PropertyDescriptor::Value("chromaLocation", GetVideoChromaLocation(), napi_enumerable),
        Napi::PropertyDescriptor::Value("fieldOrder", Napi::Number::New(Env(), (uint32_t)codecpar->field_order), napi_enumerable),
        Napi::PropertyDescriptor::Value("width", Napi::Number::New(Env(), (int)codecpar->width), napi_enumerable),
        Napi::PropertyDescriptor::Value("height", Napi::Number::New(Env(), (int)codecpar->height), napi_enumerable),
        Napi::PropertyDescriptor::Value("sar", GetVideoSar(), napi_enumerable),
        Napi::PropertyDescriptor::Value("dar", GetVideoDar(), napi_enumerable),
        Napi::PropertyDescriptor::Value("fps", GetVideoFps(), napi_enumerable),
        Napi::PropertyDescriptor::Value("tbr", GetVideoTbr(), napi_enumerable),
        Napi::PropertyDescriptor::Value("tbn", GetVideoTbn(), napi_enumerable)
    });
}


void AvStream::ExportAudioInfo(Napi::Object that) {

    const AVCodecParameters* codecpar = context->codecpar;

    that.DefineProperties({
        Napi::PropertyDescriptor::Value("frameSize",
            Napi::Number::New(Env(), (int)codecpar->frame_size), napi_enumerable),

        Napi::PropertyDescriptor::Value("sampleRate",
            Napi::Number::New(Env(), (int)codecpar->sample_rate), napi_enumerable),

        Napi::PropertyDescriptor::Value("channels",
            Napi::Number::New(Env(), (int)codecpar->channels), napi_enumerable),

        Napi::PropertyDescriptor::Value("channelLayout",
            GetAudioChannelLayout(), napi_enumerable),

        Napi::PropertyDescriptor::Value("sampleFormatName",
            GetAudioSampleFormatName(), napi_enumerable),

        Napi::PropertyDescriptor::Value("bitsPerRawSample",
            GetAudioBitsPerRawSample(), napi_enumerable),

        Napi::PropertyDescriptor::Value("delay",
            Napi::Number::New(Env(), codecpar->initial_padding), napi_enumerable),

        Napi::PropertyDescriptor::Value("padding",
            Napi::Number::New(Env(), codecpar->trailing_padding), napi_enumerable),
    });
    
}

};


