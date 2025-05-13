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

passing in probesize and analyzeduration 

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo({ filePath: 'sample.mp4', options: { probeSize: 1 * 1024 * 1024 * 1024, analyzeDuration: 1000 * 1000000 } }).then(result => {
    console.log(result);
});

```


## Build Options

You can specify `PKG_CONFIG_PATH` env variable if you want to link library with custom build of ffmpeg

```bash
export PKG_CONFIG_PATH=/path/to/ffmpeg-build/lib/pkgconfig
npm install
```

