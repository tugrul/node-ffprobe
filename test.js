
const util = require('util');

const {versions, getFileInfo, AvFormatContext, AvDuration, AvChapter, AvProgram, AvLogReader, 
    AvStreamVideo, AvStreamAudio, AvStreamData, AvStreamSubtitle, AvStreamAttachment} = require('.');

const delay = util.promisify(setTimeout);



describe('versions property', () => {

    test('should be object', () => {
        expect(typeof versions).toBe('object');
    });

    describe('should has string value on the property', () => {
        test('avcodec', () => {
            expect(typeof versions.avcodec).toBe('string');
        });

        test('avutil', () => {
            expect(typeof versions.avutil).toBe('string');
        });

        test('avformat', () => {
            expect(typeof versions.avformat).toBe('string');
        });
    });

});

describe('getFileInfo method', () => {

    describe('should throw an error for', () => {

        test('invalid type of filePath', async() => {

            const msg = 'filePath parameter should be a string value';
        
            await expect(getFileInfo()).rejects.toThrow(msg);
            await expect(getFileInfo(234)).rejects.toThrow(msg);
            await expect(getFileInfo(null)).rejects.toThrow(msg);
            await expect(getFileInfo({filePath:'test/test.mp4'})).rejects.toThrow(msg);
            await expect(getFileInfo(['test/test.mp4'])).rejects.toThrow(msg);
        });
        
        test('non-exists filePath on filesystem', async() => {
        
            const msg = 'filePath could not be opened';
        
            await expect(getFileInfo('')).rejects.toThrow(msg);
            await expect(getFileInfo('test/test')).rejects.toThrow(msg);
        });
        
        
        test('invalid media contents of a file on the filesystem', async() => {
        
            const msg = 'filePath could not be opened';
        
            await expect(getFileInfo('test/nonsense.txt')).rejects.toThrow(msg);
        });
    });

    describe('should return an object', () => {

        beforeAll(async() => {
            this.result = await Promise.all([
                getFileInfo('test/test.mp4'), getFileInfo('test/main.m3u')
            ]);
        });

        test('instance of AvFormatContext', () => {

            const [first, last] = this.result;

            expect(first).toBeInstanceOf(AvFormatContext);
            expect(last).toBeInstanceOf(AvFormatContext);

        });

        test('should have simiar structure of sample objects', () => {

            const [first, last] = this.result;

            expect(first).toMatchObject({
                name: 'mov,mp4,m4a,3gp,3g2,mj2',
                url: 'test/test.mp4',
                bitrate: 15646,
                duration: expect.any(AvDuration),
                startTime: expect.any(AvDuration),
                flags: 2097152,
                metadata: {
                    compatible_brands: 'isomiso2avc1mp41',
                    encoder: 'Lavf59.6.100',
                    major_brand: 'isom',
                    minor_version: '512'
                },
                streams: [
                    expect.any(AvStreamVideo),
                    expect.any(AvStreamAudio),
                    expect.any(AvStreamData)
                ],
                chapters: [
                    expect.any(AvChapter),
                    expect.any(AvChapter),
                    expect.any(AvChapter)
                ],
                programs: []
            });

            expect(last).toMatchObject({
                name: 'hls',
                url: 'test/main.m3u',
                bitrate: 42044,
                duration: null,
                startTime: null,
                flags: 2097152,
                metadata: {},
                streams: expect.anything(),
                chapters: [],
                programs: [
                    expect.any(AvProgram),
                    expect.any(AvProgram)
                ]
            });

        });

    });

    
});

