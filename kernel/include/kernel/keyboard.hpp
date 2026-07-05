#pragma once

#include <stddef.h>
#include <stdint.h>

namespace keyboard {
  using Key = uint8_t;
  constexpr Key none = 0;

  class Decoder {
   public:
    Key decode(uint8_t code);
  };
}
