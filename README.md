# node-ffprobe ![main workflow](https://github.com/tugrul/node-ffprobe/actions/workflows/main.yml/badge.svg)

nodejs native bindings addon of ffprobe library

## Installation

```npm install --save @tugrul/ffprobe```

## Usage

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo('sample.mp4').then(result => {
    console.log(result);
});

```

## Passing options

Available options are: probeSize, analyzeDuration, protocolWhitelist

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo('sample.mp4', { probeSize: 1 * 1024 * 1024 * 1024, analyzeDuration: 1000 * 1000000 }).then(result => {
    console.log(result);
});

```


## Restricting protocols

`getFileInfo` hands its first argument to FFmpeg, which resolves it the same way
the `ffprobe` command line tool does. That includes network protocols such as
`http:` and `rtmp:`, and playlist formats such as HLS or `.m3u` can reference
further files by URL. If the path comes from an untrusted source, restrict what
FFmpeg is allowed to open:

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo(userSuppliedPath, { protocolWhitelist: ['file'] }).then(result => {
    console.log(result);
});

```

The value maps to FFmpeg's `protocol_whitelist` option and accepts either an
array or a comma separated string. It is unset by default, so existing callers
that probe remote URLs keep working.

## Build Options

You can specify `PKG_CONFIG_PATH` env variable if you want to link library with custom build of ffmpeg

```bash
export PKG_CONFIG_PATH=/path/to/ffmpeg-build/lib/pkgconfig
npm install
```

