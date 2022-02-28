
#include "avlog-reader.h"
#include "util.h"
#include <chrono>

extern "C" {
    #include <libavutil/log.h>
}

namespace node_ffprobe {

Napi::FunctionReference AvLogReader::constructor;

int AvLogReader::printLogPrefix = 1;

std::mutex AvLogReader::logMutex;

InstanceSet AvLogReader::instances;


AvLogReader::AvLogReader(const Napi::CallbackInfo& info) : 
Napi::ObjectWrap<AvLogReader>(info), completed(false), started(false), promiseMutex(), cvPromise(), logLines(), readTimeoutMs(5000) {


    if (info[0].IsNumber()) {
        readTimeoutMs = info[0].As<Napi::Number>();
    }

    std::lock_guard<std::mutex> guard(logMutex);
    instances.insert(std::ref(*this));

}

Napi::Value AvLogReader::Next(const Napi::CallbackInfo& info) {

    Napi::HandleScope scope(Env());

    started = true;

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    if (completed) {
        deferred.Resolve(GeneratorValue(Env(), Env().Undefined(), true));
        return deferred.Promise();
    }

    AvLogReaderWorker* worker = new AvLogReaderWorker(Env(), std::move(deferred), logLines, promiseMutex, cvPromise, completed, readTimeoutMs);
    worker->Queue();

    return deferred.Promise();
}

Napi::Value AvLogReader::Return(const Napi::CallbackInfo& info) {

    Napi::HandleScope scope(Env());

    started = true;

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    if (completed) {
        deferred.Resolve(GeneratorValue(Env(), Env().Undefined(), true));
        return deferred.Promise();
    }

    completed = true;

    AvLogReaderWorker* worker = new AvLogReaderWorker(Env(), std::move(deferred), logLines, promiseMutex, cvPromise, completed, readTimeoutMs);
    worker->Queue();

    return deferred.Promise();
}

Napi::Value AvLogReader::Throw(const Napi::CallbackInfo& info) {

    Napi::HandleScope scope(Env());

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(Env());

    // throwing exception to generator is invalid if there is no yielded value before
    // we consider next or return function should be called at once before
    if (!started) {
        deferred.Reject(info[0]);
        return deferred.Promise();
    }

    if (completed) {
        deferred.Resolve(GeneratorValue(Env(), Env().Undefined(), true));
        return deferred.Promise();
    }

    completed = true;

    AvLogReaderWorker* worker = new AvLogReaderWorker(Env(), std::move(deferred), logLines, promiseMutex, cvPromise, completed, readTimeoutMs);
    worker->Queue();

    return deferred.Promise();
}


Napi::Value AvLogReader::AsyncIterator(const Napi::CallbackInfo& info) {
    return info.This();
}

void AvLogReader::LogCallback(void* ptr, int level, const char* fmt, va_list vl) {

    std::lock_guard<std::mutex> guard(logMutex);

    if (instances.empty()) {
        return;
    }

    char line[256];

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &printLogPrefix);

    for (AvLogReader& instance: instances) {
        std::lock_guard<std::mutex> guard(instance.promiseMutex);

        instance.logLines.push(line);
        instance.cvPromise.notify_one();
    }

}


Napi::Function AvLogReader::Define(Napi::Env env) {
    
    Napi::Function func = DefineClass(env, "AvLogReader", {
        InstanceMethod<&AvLogReader::Next>("next"), 
        InstanceMethod<&AvLogReader::Return>("return"),
        InstanceMethod<&AvLogReader::Throw>("throw"),
        InstanceMethod<&AvLogReader::AsyncIterator>(Napi::Symbol::WellKnown(env, "asyncIterator"))
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    return func;
}

AvLogReaderWorker::AvLogReaderWorker(const Napi::Env& env, Napi::Promise::Deferred&& def, 
    std::queue<std::string>& msg, std::mutex& mutex, std::condition_variable& con, bool& comp, const uint32_t& timeout): 
        Napi::AsyncWorker(env), deferred(std::move(def)), logLines(msg), mut(mutex), cv(con), str(), completed(comp), readTimeoutMs(timeout) {}

void AvLogReaderWorker::Execute() {
    std::unique_lock<std::mutex> lk(mut);

    // observe AvLogReader::LogCallback method until timeout delay
    completed = !cv.wait_for(lk, std::chrono::milliseconds(readTimeoutMs), [&] { return !logLines.empty(); });

    if (!completed) {
        str = logLines.front();
        logLines.pop();
    }
}

void AvLogReaderWorker::OnOK() {
    Napi::HandleScope scope(Env());

    if (completed) {
        deferred.Resolve(GeneratorValue(Env(), Env().Undefined(), true));
    } else {
        deferred.Resolve(GeneratorValue(Env(), Napi::String::New(Env(), str), false));
    }

    
}

void AvLogReaderWorker::OnError(const Napi::Error& error) {
    deferred.Reject(error.Value());
}


};