#ifndef NODE_FFPROBE_AVLOG_READER_H
#define NODE_FFPROBE_AVLOG_READER_H

#include <napi.h>



namespace node_ffprobe {

class AvLogReader: public Napi::ObjectWrap<AvLogReader> {
public:
    AvLogReader(const Napi::CallbackInfo& info);
    static Napi::Function Define(Napi::Env env);
    static Napi::FunctionReference constructor;
private:
    Napi::Value Next(const Napi::CallbackInfo& info);
    Napi::Value Return(const Napi::CallbackInfo& info);
    Napi::Value Throw(const Napi::CallbackInfo& info);
};

};


#endif
