
#pragma once

extern "C" {
    #include <libavutil/dict.h>
}

#include <string>

namespace node_ffprobe {

/**
 * RAII wrapper for FFmpeg's AVDictionary
 * Provides automatic memory management and convenient methods
 * for dictionary operations.
 */
class AvDictionary {
public:
    // Default constructor creates an empty dictionary
    AvDictionary() : m_dict(nullptr) {}

    // Constructor from existing AVDictionary pointer (takes ownership)
    explicit AvDictionary(AVDictionary* dict) : m_dict(dict) {}

    // Move constructor
    AvDictionary(AvDictionary&& other) noexcept : m_dict(other.m_dict) {
        other.m_dict = nullptr;
    }

    // Move assignment operator
    AvDictionary& operator=(AvDictionary&& other) noexcept {
        if (this != &other) {
            free();
            m_dict = other.m_dict;
            other.m_dict = nullptr;
        }
        return *this;
    }

    // Destructor automatically frees the dictionary
    ~AvDictionary() {
        free();
    }

    // Delete copy constructor and assignment operator
    AvDictionary(const AvDictionary&) = delete;
    AvDictionary& operator=(const AvDictionary&) = delete;

    // Set a key-value pair in the dictionary
    int set(const std::string& key, const std::string& value, int flags = 0) {
        return av_dict_set(&m_dict, key.c_str(), value.c_str(), flags);
    }

    // Set a key with integer value in the dictionary
    int set(const std::string& key, int64_t value, int flags = 0) {
        return av_dict_set_int(&m_dict, key.c_str(), value, flags);
    }

    // Manually free the dictionary
    void free() {
        if (m_dict) {
            av_dict_free(&m_dict);
            m_dict = nullptr;
        }
    }

    // Get the underlying AVDictionary pointer
    AVDictionary* getBareInstance() const {
        return m_dict;
    }

    // Allow implicit casting to AVDictionary** for use in FFmpeg functions
    operator AVDictionary**() {
        return &m_dict;
    }

    // Allow implicit casting to AVDictionary* for use in FFmpeg functions
    operator AVDictionary*() const {
        return m_dict;
    }

private:
    AVDictionary* m_dict;
};

};
