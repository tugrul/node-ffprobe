
#ifndef NODE_FFPROBE_SHARED_SLOT_H
#define NODE_FFPROBE_SHARED_SLOT_H

namespace node_ffprobe {

/**
 * Publishes a pointer in a shared slot for the duration of a scope and clears
 * it again on the way out, including when the scope is left via an exception.
 *
 * The wrapper classes receive their payload through the data pointer handed to
 * DefineClass(), so the payload has to be reachable from a known location while
 * the JS constructor runs. node-addon-api enables C++ exceptions whenever the
 * compiler has them enabled, which means those constructors can throw; leaving
 * a stale pointer behind would let a later direct construction from JS wrap
 * memory that has since been freed.
 */
template <typename T>
class SharedSlot {
public:
    SharedSlot(T** slot, T* value) : slot(slot) {
        *slot = value;
    }

    ~SharedSlot() {
        *slot = nullptr;
    }

    SharedSlot(const SharedSlot&) = delete;
    SharedSlot& operator=(const SharedSlot&) = delete;

private:
    T** slot;
};

};

#endif
