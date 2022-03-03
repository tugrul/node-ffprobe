

#ifndef NODE_FFPROBE_UTIL_H
#define NODE_FFPROBE_UTIL_H

#include <map>
#include <vector>
#include <napi.h>

extern "C" {
    #include <libavutil/dict.h>
}

namespace node_ffprobe {

const Napi::Object GetMetadata(const Napi::Env& env, const AVDictionary* dictionary);
const Napi::Object GeneratorValue(Napi::Env env, Napi::Value value, bool done);

};

#endif
