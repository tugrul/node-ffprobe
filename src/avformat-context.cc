
#include "avformat-context.h"
#include "media-info-worker.h"

#include "instance-data.h"
#include "util.h"
#include "avduration.h"
#include "avchapter.h"
#include "avprogram.h"
#include <iostream>


namespace node_ffprobe {

AvStreamContext* AvFormatContext::sharedContext = nullptr;
std::mutex AvFormatContext::sharedContextMutex;

Napi::FunctionReference AvFormatContext::constructor;

AvFormatContext::AvFormatContext(const Napi::CallbackInfo& info) :
    Napi::ObjectWrap<AvFormatContext>(info) {

        if (info.Data() == nullptr) {
            Napi::Error::New(Env(), "CallbackInfo::Data should have AVFormatContext").ThrowAsJavaScriptException();
            return;
        }

        context = *static_cast<AVFormatContext**>(info.Data());

        if (context == nullptr) {
            Napi::Error::New(Env(), "Direct initialization is not allowed for this wrapper class").ThrowAsJavaScriptException();
            return;
        }

        Napi::Object that = info.This().As<Napi::Object>();

        ExportCommonInfo(that);
        Napi::Array streams = ExportStreamInfo(that);
        ExportChapterInfo(that);
        ExportProgramInfo(that, streams);

}

AvFormatContext::~AvFormatContext() {
    avformat_close_input(&context);
}

void AvFormatContext::ExportCommonInfo(Napi::Object that) {

    that.DefineProperties({

        Napi::PropertyDescriptor::Value("name",
            Napi::String::New(Env(), context->iformat->name), napi_enumerable), 
        
        Napi::PropertyDescriptor::Value("url",
            Napi::String::New(Env(), context->url), napi_enumerable), 
        
        Napi::PropertyDescriptor::Value("bitrate",
            Napi::Number::New(Env(), context->bit_rate), napi_enumerable), 

        Napi::PropertyDescriptor::Value("duration",
            context->duration == AV_NOPTS_VALUE ? Env().Null() : AvDuration::constructor.New({Napi::Number::New(Env(), context->duration)}), 
            napi_enumerable), 
            
        Napi::PropertyDescriptor::Value("startTime",
            context->start_time == AV_NOPTS_VALUE ? Env().Null() : AvDuration::constructor.New({Napi::Number::New(Env(), context->start_time)}), 
            napi_enumerable),

        Napi::PropertyDescriptor::Value("flags",
            Napi::Number::New(Env(), context->flags), napi_enumerable), 
            
        Napi::PropertyDescriptor::Value("metadata",
            getMetadata(Env(), context->metadata), napi_enumerable)

    });

}

Napi::Array AvFormatContext::ExportStreamInfo(Napi::Object that) {

    Napi::Array streams = Napi::Array::New(Env(), (size_t)context->nb_streams);

    std::lock_guard<std::mutex> guard(sharedContextMutex);

    sharedContext = new AvStreamContext({.format= context->iformat, .context = nullptr});

    for (size_t i = 0; i < context->nb_streams; i++) {

        sharedContext->context = context->streams[i];
        
        switch (sharedContext->context->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
            streams[i] = AvStream::consVideo.New({});
            break;

            case AVMEDIA_TYPE_AUDIO:
            streams[i] = AvStream::consAudio.New({});
            break;

            case AVMEDIA_TYPE_UNKNOWN:
            case AVMEDIA_TYPE_DATA:
            streams[i] = AvStream::consData.New({});
            break;

            case AVMEDIA_TYPE_SUBTITLE:
            streams[i] = AvStream::consSubtitle.New({});
            break;

            case AVMEDIA_TYPE_ATTACHMENT:
            streams[i] = AvStream::consAttachment.New({});
            break;

            default:
            sharedContext->context = nullptr;
        }
    }

    delete sharedContext;
    sharedContext = nullptr;

    that.DefineProperty(Napi::PropertyDescriptor::Value("streams", streams, napi_enumerable));

    return streams;
}

void AvFormatContext::ExportChapterInfo(Napi::Object that) {

    Napi::Array chapters = Napi::Array::New(Env(), (size_t)context->nb_chapters);

    for (size_t i = 0; i < context->nb_chapters; i++) {

        const AVChapter* chapter = context->chapters[i];

        double timeBase = av_q2d(chapter->time_base);
        
        chapters[i] = AvChapter::constructor.New({
            Napi::Number::New(Env(), chapter->id),
            Napi::Number::New(Env(), chapter->start * timeBase),
            Napi::Number::New(Env(), chapter->end * timeBase),
            getMetadata(Env(), chapter->metadata)
        });
    }

    that.DefineProperty(Napi::PropertyDescriptor::Value("chapters", chapters, napi_enumerable));
}

void AvFormatContext::ExportProgramInfo(Napi::Object that, Napi::Array streams) {
    Napi::Array programs = Napi::Array::New(Env(), (size_t)context->nb_programs);

    for (size_t i = 0; i < context->nb_programs; i++) {

        const AVProgram* program = context->programs[i];

        Napi::Array progStreams = Napi::Array::New(Env(), (size_t)program->nb_stream_indexes);
        size_t streamIndex = 0;

        for (size_t j = 0; j < program->nb_stream_indexes; j++) {
            Napi::Value target = streams.Get(program->stream_index[j]);
            if (target.IsObject()) {
                progStreams[streamIndex++] = target;
            }
        }

        programs[i] = AvProgram::constructor.New({
            Napi::Number::New(Env(), program->id),
            Napi::Number::New(Env(), program->flags),
            Napi::Number::New(Env(), program->discard),
            progStreams,
            getMetadata(Env(), program->metadata),
            Napi::Number::New(Env(), program->program_num),
            Napi::Number::New(Env(), program->pmt_pid),
            Napi::Number::New(Env(), program->pcr_pid),
            Napi::Number::New(Env(), program->pmt_version)
        });

    }

    that.DefineProperty(Napi::PropertyDescriptor::Value("programs", programs, napi_enumerable));
}

Napi::Function AvFormatContext::Define(Napi::Env env, AVFormatContext* data) {

    Napi::Function func = DefineClass(env, "AvFormatContext", {}, &MediaInfoWorker::sharedContext);

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    // InstanceData* instanceData = new InstanceData();
    // env.SetInstanceData<InstanceData>(instanceData);
    // instanceData->avFormatContext = Napi::Persistent(avFunc);

    return func;

}

};

