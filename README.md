# node-ffprobe

## Usage

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo('sample.mp4').then(result => {
    console.log(result);
});

```


## Build Options

You can specify `PKG_CONFIG_PATH` env variable if you want to link library with custom build of ffmpeg

```bash
export PKG_CONFIG_PATH=/path/to/ffmpeg-build/lib/pkgconfig
npm install
```

