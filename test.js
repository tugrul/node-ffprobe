
const util = require('util');

const {versions, getFileInfo, getLogReader, AvFormatContext, AvDuration, AvChapter, AvProgram, AvLogReader, 
    AvStreamVideo, AvStreamAudio, AvStreamData, AvStreamSubtitle, AvStreamAttachment} = require('.');

const delay = util.promisify(setTimeout);



describe('versions property', () => {

    test('should be an object type', () => {
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
            await expect(getFileInfo({filePath:'test-assets/test.mp4'})).rejects.toThrow(msg);
            await expect(getFileInfo(['test-assets/test.mp4'])).rejects.toThrow(msg);
        });
        
        test('non-exists filePath on filesystem', async() => {
        
            const msg = 'filePath could not be opened';
        
            await expect(getFileInfo('')).rejects.toThrow(msg);
            await expect(getFileInfo('test-assets/test')).rejects.toThrow(msg);
        });
        
        
        test('invalid media contents of a file on the filesystem', async() => {
        
            const msg = 'filePath could not be opened';
        
            await expect(getFileInfo('test-assets/nonsense.txt')).rejects.toThrow(msg);
        });
    });

    describe('should return an object', () => {

        beforeAll(async() => {
            this.result = await Promise.all([
                getFileInfo('test-assets/test.mp4'), getFileInfo('test-assets/main.m3u')
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
                url: 'test-assets/test.mp4',
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
                url: 'test-assets/main.m3u',
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

describe('AvChapter class', () => {

    describe('throw an TypeError from constructor', () => {


        test('when chapter id parameter is not a number type', () => {
            const msg = 'Chapter ID value should be numeric';

            for (const value of [undefined, null, '123', [], {}]) {
                expect(() => {
                    new AvChapter(value);
                }).toThrow(msg);
            }
        });

        test('when chapter start parameter is not a number type', () => {
            const msg = 'Chapter start time should be numeric';

            for (const value of [undefined, null, '123', [], {}]) {
                expect(() => {
                    new AvChapter(123, value);
                }).toThrow(msg);
            }
        });

        test('when chapter end parameter is not a number type', () => {
            const msg = 'Chapter end time should be numeric';

            for (const value of [undefined, null, '123', [], {}]) {
                expect(() => {
                    new AvChapter(123, 456, value);
                }).toThrow(msg);
            }
        });

        test('when metadata parameter is not an object type', () => {
            const msg = 'Metadata parameter type is invalid';

            for (const value of [null, '123', []]) {
                expect(() => {
                    new AvChapter(123, 456, 789, value);
                }).toThrow(msg);
            }
        });

        test('should have simiar structure of sample objects', () => {

            const chapter1 = new AvChapter(123, 456, 789, {
                bam: 'bim'
            });

            expect(chapter1).toMatchObject({
                id: 123,
                start: 456,
                end: 789,
                metadata: {
                    bam: 'bim'
                }
            });

            const chapter2 = new AvChapter(222, 444, 666);

            expect(chapter2).toMatchObject({
                id: 222,
                start: 444,
                end: 666,
                metadata: {}
            });
        });

    });

    
    

});

describe('AvDuration class', () => {

    test('throw an TypeError from constructor for non-numeric parameter', () => {

        const msg = 'A number was expected';

        expect(() => {
            new AvDuration();
        }).toThrow(msg);

        for (const value of [undefined, null, '123', [], {}]) {
            expect(() => {
                new AvDuration(value);
            }).toThrow(msg);
        }

    });

    describe('instance members', () => {

        const timeSerial = 9876543210;

        const duration = new AvDuration(timeSerial);

        test('valueOf method', () => {
            expect(duration.valueOf()).toBe(timeSerial);
        });

        test('toString method', () => {
            expect(duration.toString()).toBe('02:44:36.54');
        });

        test('field values', () => {
            expect(duration).toMatchObject({
                hours: 2,
                minutes: 44,
                seconds: 36,
                microseconds: 54
            });
        });

    });

});

describe('AvFormatContext class', () => {

    test('throw an Error from constructor because it is private', () => {

        const msg = 'Direct initialization is not allowed for this wrapper class';

        expect(() => {
            new AvFormatContext();
        }).toThrow(msg);

    });

});

describe('AvLogReader class', () => {

    test('should return a log message', async () => {


        const getFirstLogLine = async() => {
            const reader = getLogReader(5000);
            for await (const line of reader) {
                return line;
            }
        };

        const [msg] = await Promise.all([
            getFirstLogLine(),
            getFileInfo('test-assets/test.mp4')
        ]);


    });



});
