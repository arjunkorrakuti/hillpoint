#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/runtime.hpp>
#include <stddef.h>

namespace {
  struct __attribute__((packed)) Descriptor {
    uint16_t limit;
    uint64_t address;
  };

  struct __attribute__((packed)) Gate {
    uint16_t low;
    uint16_t selector;
    uint8_t stack;
    uint8_t attributes;
    uint16_t middle;
    uint32_t high;
    uint32_t reserved;
  };

  static_assert(sizeof(Gate) == 16);
  static_assert(offsetof(interrupts::Frame, vector) == 15 * 8);
  alignas(16) Gate gates[32] = {};
  alignas(16) uint64_t gdt[3] = {0, 0x00af9a000000ffff, 0x00cf92000000ffff};
}

extern "C" void loadGdt(const Descriptor* descriptor);
extern "C" const uintptr_t interruptStubs[32];

void interrupts::initialize() {
  asm volatile("cli" ::: "memory");
  const Descriptor gdtDescriptor = {sizeof(gdt) - 1, reinterpret_cast<uintptr_t>(gdt)};
  loadGdt(&gdtDescriptor);
  for (size_t index = 0; index < 32; index++) {
    const uintptr_t handler = interruptStubs[index];
    gates[index] = {.low = static_cast<uint16_t>(handler),
                    .selector = 0x08,
                    .stack = 0,
                    .attributes = 0x8e,
                    .middle = static_cast<uint16_t>(handler >> 16),
                    .high = static_cast<uint32_t>(handler >> 32),
                    .reserved = 0};
  }
  const Descriptor idt = {.limit = sizeof(gates) - 1,
                          .address = reinterpret_cast<uintptr_t>(gates)};
  asm volatile("lidt %0" : : "m"(idt) : "memory");
}

extern "C" void interruptDispatch(const interrupts::Frame* frame) {
  console::printf("Exception %llu at %llx\n",
                  static_cast<unsigned long long>(frame->vector),
                  static_cast<unsigned long long>(frame->rip));
  panic("Unhandled CPU exception");
}
