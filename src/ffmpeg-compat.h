
#ifndef NODE_FFPROBE_FFMPEG_COMPAT_H
#define NODE_FFPROBE_FFMPEG_COMPAT_H

/**
 * Compatibility shims for API changes across FFmpeg major releases.
 *
 * The addon supports FFmpeg 4.x through 7.x/8.x. Where an API was replaced,
 * the newer form is preferred whenever it is available, so that builds against
 * intermediate releases (where both forms exist) stay warning free.
 */

#include <string>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/avutil.h>
    #include <libavutil/channel_layout.h>
}

/*
 * The AVChannelLayout API landed in FFmpeg 5.1 (libavcodec 59.24.100).
 * The legacy AVCodecParameters::channels / ::channel_layout fields and
 * av_get_channel_layout_string() were deprecated at the same time and
 * removed outright in FFmpeg 7.0 (libavcodec 61).
 */
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(59, 24, 100)
    #define NODE_FFPROBE_HAVE_CH_LAYOUT 1
#else
    #define NODE_FFPROBE_HAVE_CH_LAYOUT 0
#endif

/*
 * FF_LEVEL_UNKNOWN was renamed to AV_LEVEL_UNKNOWN in FFmpeg 7.1 and the old
 * spelling is deprecated. Both are plain macros, so a definedness check picks
 * the right one.
 */
#if !defined(AV_LEVEL_UNKNOWN)
    #define AV_LEVEL_UNKNOWN FF_LEVEL_UNKNOWN
#endif

namespace node_ffprobe {

/** Number of audio channels described by the given codec parameters. */
inline int GetChannelCount(const AVCodecParameters* codecpar) {
#if NODE_FFPROBE_HAVE_CH_LAYOUT
    return codecpar->ch_layout.nb_channels;
#else
    return codecpar->channels;
#endif
}

/**
 * Human readable channel layout, e.g. "stereo" or "5.1(side)".
 * Returns an empty string when the layout cannot be described.
 */
inline std::string GetChannelLayoutString(const AVCodecParameters* codecpar) {

    char buf[256];

#if NODE_FFPROBE_HAVE_CH_LAYOUT
    const int written = av_channel_layout_describe(&codecpar->ch_layout, buf, sizeof(buf));

    if (written < 0) {
        return std::string();
    }
#else
    av_get_channel_layout_string(buf, sizeof(buf), codecpar->channels, codecpar->channel_layout);
#endif

    buf[sizeof(buf) - 1] = '\0';

    return std::string(buf);
}

};

#endif
