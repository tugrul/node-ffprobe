
#include "util.h"

namespace node_ffprobe {

const Napi::Object getMetadata(const Napi::Env& env, const AVDictionary* dictionary) {

    std::map<std::string, std::vector<std::string>> items;

    Napi::Object metadata = Napi::Object::New(env);

    const AVDictionaryEntry *tag = NULL;

    while ((tag = av_dict_get(dictionary, "", tag, AV_DICT_IGNORE_SUFFIX))) {
        items[tag->key].push_back(tag->value);
    }

    for (const auto& target : items) {

        if (target.second.size() == 1) {
            metadata.DefineProperty(Napi::PropertyDescriptor::Value(Napi::String::New(env, target.first), 
                Napi::String::New(env, target.second[0]), napi_enumerable));
            continue;
        }

        Napi::Array item = Napi::Array::New(env, target.second.size());

        for (size_t index = 0; index < target.second.size(); index++) {
            item[index] = Napi::String::New(env, target.second[index]);
        }

        metadata.DefineProperty(Napi::PropertyDescriptor::Value(Napi::String::New(env, target.first), 
            item, napi_enumerable));
    }
 
    return metadata;
}

const Napi::Object GeneratorValue(Napi::Env env, Napi::Value value, bool done) {

    Napi::Object ret = Napi::Object::New(env);

    ret.Set(Napi::String::New(env, "value"), value);
    ret.Set(Napi::String::New(env, "done"), Napi::Boolean::New(env, done));

    return ret;
}

};
