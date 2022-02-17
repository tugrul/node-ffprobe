# node-ffprobe

## Usage

```javascript

const ffprobe = require('@tugrul/ffprobe');

ffprobe.getFileInfo('sample.mp4').then(result => {
    console.log(result);
});

```