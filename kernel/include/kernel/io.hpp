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

  inline void wait() {
    out(0x80, 0);
  }

  inline uint64_t disableInterrupts() {
    uint64_t flags;
    asm volatile("pushfq; popq %0; cli" : "=r"(flags) : : "memory");
    return flags;
  }

  inline void restoreInterrupts(uint64_t flags) {
    if ((flags & (1U << 9)) != 0) {
      asm volatile("sti" ::: "memory");
    }
  }
}
