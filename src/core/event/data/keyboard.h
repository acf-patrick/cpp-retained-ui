#pragma once

#include <raylib.h>

namespace event {
namespace data {

struct KeyDown {
    KeyboardKey key;
    bool repeat; // caused by the key being held down, rather than the first initial press.
};

struct KeyUp {
    KeyboardKey key;
    bool repeat; // caused by the key being held down, rather than the first initial press.
};

} // namespace data
} // namespace event