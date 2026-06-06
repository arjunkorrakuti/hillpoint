#pragma once

#include <stdint.h>

namespace io {
  inline void out(uint16_t port, uint8_t value) {
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
  }

  inline uint8_t in(uint16_t port) {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
  }
}
