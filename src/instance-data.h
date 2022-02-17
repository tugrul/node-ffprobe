
#ifndef NODE_FFPROBE_INSTANCE_DATA_H
#define NODE_FFPROBE_INSTANCE_DATA_H

#include <napi.h>

namespace node_ffprobe {
    typedef struct InstanceData
    {
        Napi::FunctionReference avFormatContext;
    } InstanceData;
}

#endif
