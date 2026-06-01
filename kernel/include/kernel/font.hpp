#pragma once

#include <stddef.h>
#include <stdint.h>

namespace font {
  constexpr size_t width = 12;
  constexpr size_t height = 16;
  bool pixel(unsigned char character, size_t x, size_t y);
}
