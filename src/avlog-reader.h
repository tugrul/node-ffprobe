#ifndef NODE_FFPROBE_AVLOG_READER_H
#define NODE_FFPROBE_AVLOG_READER_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <queue>
#include <tuple>

#include <napi.h>

namespace node_ffprobe {

class AvLogReader;


class AvLogReader: public Napi::ObjectWrap<AvLogReader> {
public:
    AvLogReader(const Napi::CallbackInfo& info);
    static Napi::Function Define(Napi::Env env);
    static Napi::FunctionReference constructor;
    bool Push(const int& level, const std::string& message);
private:

    std::queue<std::tuple<int, std::string>> logLines;
    std::mutex promiseMutex;
    std::condition_variable cvPromise;
    bool completed;
    bool started;
    uint32_t readTimeoutMs;
    Napi::Value Next(const Napi::CallbackInfo& info);
    Napi::Value Return(const Napi::CallbackInfo& info);
    Napi::Value Throw(const Napi::CallbackInfo& info);
    Napi::Value AsyncIterator(const Napi::CallbackInfo& info);
    Napi::Value Push(const Napi::CallbackInfo& info);
};


class AvLogReaderWorker : public Napi::AsyncWorker {
public:
    AvLogReaderWorker(const Napi::Env& env, Napi::Promise::Deferred&& def, std::queue<std::tuple<int, std::string>>& msg, 
        std::mutex& mutex, std::condition_variable& con, bool& comp, const uint32_t& timeout);
    void Execute() override;
    void OnOK() override;
    void OnError(const Napi::Error& error) override;

private:
    Napi::Promise::Deferred deferred;
    std::queue<std::tuple<int, std::string>>& logLines;
    std::mutex& mut;
    std::condition_variable& cv;
    std::string str;
    int level;
    bool& completed;
    const uint32_t& readTimeoutMs; 
};
 

};


#endif
