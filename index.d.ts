export declare class AvDuration {
    constructor(timeserial: number);
    toString(): string;
    valueOf(): number;
    readonly hours: number;
    readonly minutes: number;
    readonly seconds: number;
    readonly microseconds: number;
}
export interface Disposition {
    readonly 'default': boolean;
    readonly dub: boolean;
    readonly original: boolean;
    readonly comment: boolean;
    readonly lyrics: boolean;
    readonly karaoke: boolean;
    readonly forced: boolean;
    readonly hearingImpaired: boolean;
    readonly visualImpaired: boolean;
    readonly cleanEffects: boolean;
    readonly attachedPic: boolean;
    readonly timedThumbnails: boolean;
    readonly captions: boolean;
    readonly descriptions: boolean;
    readonly metadata: boolean;
    readonly dependent: boolean;
    readonly stillImage: boolean;
}
export interface Metadata {
    [key: string]: string | string[];
}
export interface StreamBase {
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
}
export declare type AvRational = [number, number];
export declare class StreamVideo implements StreamBase {
    private constructor();
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
    readonly pixFmt: string | null;
    readonly bpc: number | null;
    readonly colorRange: string | null;
    readonly colorSpace: string | null;
    readonly colorPrimaries: string | null;
    readonly colorTransferCharacteristic: string | null;
    readonly chromaLocation: string | null;
    readonly fieldOrder: number;
    readonly width: number;
    readonly height: number;
    readonly sar: AvRational;
    readonly dar: AvRational;
    readonly fps: number | null;
    readonly tbr: number | null;
    readonly tbn: number | null;
}
export declare class StreamAudio implements StreamBase {
    private constructor();
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
    readonly frameSize: number;
    readonly sampleRate: number;
    readonly channels: number;
    readonly channelLayout: string;
    readonly sampleFormatName: string | null;
    readonly bitsPerRawSample: number | null;
    readonly delay: number;
    readonly padding: number;
}
export declare class StreamData implements StreamBase {
    private constructor();
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
    readonly tbn: AvRational | null;
}
export declare class StreamSubtitle implements StreamBase {
    private constructor();
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
    readonly width: number;
    readonly height: number;
}
export declare class StreamAttachment implements StreamBase {
    private constructor();
    readonly id: number | null;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly codecTag: string | null;
    readonly bitrate: number;
    readonly profile: string | null;
    readonly level: number | null;
    readonly codecName: string;
    readonly disposition: Disposition;
    readonly metadata: Metadata;
}
export declare type Stream = StreamVideo | StreamAudio | StreamData | StreamSubtitle | StreamAttachment;
export declare class AvChapter {
    constructor(id: number, start: number, end: number, metadata?: Metadata);
    readonly id: number;
    readonly start: number;
    readonly end: number;
    readonly metadata?: Metadata;
}
export declare class AvProgram {
    constructor(programId: number, flags: number, discard: number, streams: Stream[], metadata?: Metadata, programNum?: number, pmtPid?: number, pcrPid?: number, pmtVersion?: number);
    readonly id: number;
    readonly flags: number;
    readonly discard: number;
    readonly streams: Stream[];
    readonly metadata?: Metadata;
    readonly programNum?: number;
    readonly pmtPid?: number;
    readonly pcrPid?: number;
    readonly pmtVersion?: number;
}
export declare class AvFormatContext {
    private constructor();
    readonly name: string;
    readonly url: string;
    readonly bitrate: number;
    readonly duration: AvDuration | null;
    readonly startTime: AvDuration | null;
    readonly flags: number;
    readonly metadata: Metadata;
    readonly streams: Stream[];
    readonly chapters: AvChapter[];
    readonly programs: AvProgram[];
}
export interface Versions {
    readonly avformat: string;
    readonly avcodec: string;
    readonly avutil: string;
}
export declare enum LogLevel {
    Panic = 0,
    Fatal = 8,
    Error = 16,
    Warning = 24,
    Info = 32,
    Verbose = 40,
    Debug = 48,
    Trace = 56
}
export declare type LogMessage = [LogLevel, string];
export declare class AvLogReader implements AsyncIterableIterator<LogMessage> {
    constructor(timeout: number);
    [Symbol.asyncIterator](): AsyncIterableIterator<LogMessage>;
    next(): Promise<IteratorResult<LogMessage, any>>;
    return(value?: any): Promise<IteratorResult<LogMessage, any>>;
    throw(e?: any): Promise<IteratorResult<LogMessage, any>>;
}
export declare const versions: Versions;
export declare function getFileInfo(filePath: string): Promise<AvFormatContext>;
