#pragma once

#include <stdint.h>

namespace interrupts {
  struct Frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t vector, error, rip, cs, flags, rsp, ss;
  };

  using Handler = void (*)();
  void initialize();
  bool registerIrq(uint8_t irq, Handler handler);
  uint64_t count(uint8_t irq);
}
