#pragma once

#include <stdint.h>

namespace timer {
  constexpr uint32_t frequency = 100;
  void initialize();
  uint64_t ticks();
}
