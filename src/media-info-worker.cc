
#include "media-info-worker.h"


namespace node_ffprobe {

AVFormatContext* MediaInfoWorker::sharedContext = nullptr;
std::mutex MediaInfoWorker::sharedContextMutex;

MediaInfoWorker::MediaInfoWorker(const Napi::Env& env, const std::string& fileName): 
        Napi::AsyncWorker(env), 
        fileName(fileName), 
        deferred(Napi::Promise::Deferred::New(env)) {}


//~MediaInfoWorker() {
    //    avformat_close_input(&avFormatContext);
    //}

void MediaInfoWorker::Execute() {

    avFormatContext = avformat_alloc_context();

    if (!avFormatContext) {
        Napi::AsyncWorker::SetError("aformat context memory could not allocated");
        return;
    }

    int openStatus = avformat_open_input(&avFormatContext, fileName.c_str(), NULL, NULL);

    if (openStatus < 0) {
        Napi::AsyncWorker::SetError("file could not opened");
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

    // Napi::Array streams = Napi::Array::New(Env(), (int)avFormatContext->nb_streams);


    // Napi::FunctionReference* constructor = &Env().GetInstanceData<InstanceData>()->avFormatContext;


    // constructor->New()

    /*for (size_t i = 0; i < avFormatContext->nb_streams; i++) {

        AVStream* avStream = avFormatContext->streams[i];
        AVCodecParameters* codecpar = avStream->codecpar;

        Napi::Object stream = Napi::Object::New(Env());

        stream.Set(Napi::String::New(Env(), "id"), 
                Napi::Number::New(Env(), avStream->id));

        stream.Set(Napi::String::New(Env(), "startTime"), 
                Napi::Number::New(Env(), avStream->start_time));

        stream.Set(Napi::String::New(Env(), "duration"), 
                Napi::Number::New(Env(), avStream->duration));

        stream.Set(Napi::String::New(Env(), "codecName"), 
                Napi::String::New(Env(), reinterpret_cast<char*>(&codecpar->codec_tag), 4));

        stream.Set(Napi::String::New(Env(), "codecType"), 
                Napi::Number::New(Env(), codecpar->codec_type));

        stream.Set(Napi::String::New(Env(), "format"), 
                Napi::String::New(Env(), av_get_pix_fmt_name(static_cast<AVPixelFormat>(codecpar->format))));

        stream.Set(Napi::String::New(Env(), "bitrate"), 
                Napi::Number::New(Env(), codecpar->bit_rate));

        stream.Set(Napi::String::New(Env(), "codec_id"), 
                Napi::Number::New(Env(), codecpar->codec_id));

        //stream.Set(Napi::String::New(Env(), "profile"), 
        //		Napi::String::New(Env(), avcodec_profile_name(codecpar->codec_id, codecpar->profile)));

        stream.Set(Napi::String::New(Env(), "level"), 
                Napi::Number::New(Env(), (int)codecpar->level));

        stream.Set(Napi::String::New(Env(), "width"), 
                Napi::Number::New(Env(), (int)codecpar->width));

        stream.Set(Napi::String::New(Env(), "height"), 
                Napi::Number::New(Env(), (int)codecpar->height));

        stream.Set(Napi::String::New(Env(), "channels"), 
                Napi::Number::New(Env(), (int)codecpar->channels));

        stream.Set(Napi::String::New(Env(), "sampleRate"), 
                Napi::Number::New(Env(), (int)codecpar->sample_rate));

        stream.Set(Napi::String::New(Env(), "frameSize"), 
                Napi::Number::New(Env(), (int)codecpar->frame_size));

        stream.Set(Napi::String::New(Env(), "metadata"), 
            getMetadata(Env(), avStream->metadata));

        streams[i] = stream;
    }
    */

    // Napi::Object result = Napi::Object::New(Env());

/*
    result.Set(Napi::String::New(Env(), "name"), 
            Napi::String::New(Env(), avFormatContext->iformat->name));

    result.Set(Napi::String::New(Env(), "url"), 
            Napi::String::New(Env(), avFormatContext->url));

    result.Set(Napi::String::New(Env(), "bitrate"), 
            Napi::Number::New(Env(), (int)avFormatContext->bit_rate));

    result.Set(Napi::String::New(Env(), "duration"), 
            Napi::Number::New(Env(), (int)avFormatContext->duration));


    result.Set(Napi::String::New(Env(), "flags"), 
            Napi::Number::New(Env(), avFormatContext->flags));

    result.Set(Napi::String::New(Env(), "streams"), 
            streams);

    result.Set(Napi::String::New(Env(), "metadata"), 
            getMetadata(Env(), avFormatContext->metadata));

    */

    deferred.Resolve(result);
}


void MediaInfoWorker::OnError(const Napi::Error& error) {
    deferred.Reject(error.Value());
}


Napi::Promise MediaInfoWorker::GetPromise() {
    return deferred.Promise();
}

};