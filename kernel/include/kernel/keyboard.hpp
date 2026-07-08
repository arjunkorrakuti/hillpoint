#pragma once

#include <stddef.h>
#include <stdint.h>

namespace keyboard {
  using Key = uint8_t;
  constexpr Key none = 0;

  class Decoder {
   public:
    Key decode(uint8_t code);

   private:
    bool extended = false;
    uint8_t pauseBytes = 0;
  };

  class Queue {
   public:
    bool push(Key key);
    bool pop(Key& key);

   private:
    Key keys[128] = {};
    size_t readIndex = 0;
    size_t writeIndex = 0;
  };

}
