
#ifndef NODE_FFPROBE_INSTANCE_DATA_H
#define NODE_FFPROBE_INSTANCE_DATA_H

#include <napi.h>

extern "C" {
    #include <libavformat/avformat.h>
}

namespace node_ffprobe {

struct AvStreamContext;

/**
 * Per-environment state for the addon.
 *
 * All of this used to live in static members, which is only correct while
 * exactly one Node.js environment exists. References belong to the napi_env
 * that created them, so a second environment loading the addon -- a
 * worker_thread, or an Electron renderer -- overwrote the first one's
 * constructors, after which the first environment was calling references
 * owned by another environment.
 *
 * The struct is registered with napi_set_instance_data and destroyed together
 * with the environment that owns it.
 */
struct InstanceData {

    // Constructors of the exported wrapper classes.
    Napi::FunctionReference avFormatContext;
    Napi::FunctionReference avDuration;
    Napi::FunctionReference avChapter;
    Napi::FunctionReference avProgram;
    Napi::FunctionReference avStreamVideo;
    Napi::FunctionReference avStreamAudio;
    Napi::FunctionReference avStreamData;
    Napi::FunctionReference avStreamSubtitle;
    Napi::FunctionReference avStreamAttachment;

    // Payload slots that the wrapper constructors read back through
    // CallbackInfo::Data(). Each environment has its own pair and only ever
    // touches them from its own thread, so the mutexes that used to guard the
    // static versions are no longer needed.
    AVFormatContext* formatContextSlot = nullptr;
    AvStreamContext* streamContextSlot = nullptr;
};

inline InstanceData* GetInstanceData(Napi::Env env) {
    return env.GetInstanceData<InstanceData>();
}

};

#endif
