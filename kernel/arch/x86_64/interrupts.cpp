#include <cpuid.h>
#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/io.hpp>
#include <kernel/runtime.hpp>
#include <stddef.h>
#include <stdint.h>

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

  struct __attribute__((packed)) TaskState {
    uint32_t reserved0;
    uint64_t rsp[3];
    uint64_t reserved1;
    uint64_t stacks[7];
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t ioMap;
  };

  static_assert(sizeof(Gate) == 16);
  static_assert(sizeof(TaskState) == 104);
  static_assert(offsetof(interrupts::Frame, vector) == 15 * 8);
  alignas(16) Gate gates[256] = {};
  alignas(16) uint64_t gdt[5] = {0, 0x00af9a000000ffff, 0x00cf92000000ffff, 0, 0};
  alignas(16) TaskState task = {};
  alignas(16) uint8_t faultStack[16384];
  interrupts::Handler handlers[16] = {};
  uint64_t counts[16] = {};

  const char* const exceptions[32] = {"Divide error",
                                      "Debug",
                                      "Non-maskable interrupt",
                                      "Breakpoint",
                                      "Overflow",
                                      "Bounds",
                                      "Invalid opcode",
                                      "Device unavailable",
                                      "Double fault",
                                      "Reserved",
                                      "Invalid TSS",
                                      "Segment not present",
                                      "Stack fault",
                                      "General protection",
                                      "Page fault",
                                      "Reserved",
                                      "x87 error",
                                      "Alignment check",
                                      "Machine check",
                                      "SIMD error",
                                      "Virtualization",
                                      "Control protection",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Reserved",
                                      "Hypervisor injection",
                                      "VMM communication",
                                      "Security",
                                      "Reserved"};

  void remapPic() {
    unsigned int eax, ebx, ecx, edx;
    __cpuid(1, eax, ebx, ecx, edx);
    if ((edx & (1U << 9)) != 0) {
      uint32_t low, high;
      asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1b));
      low &= ~((1U << 11) | (1U << 10));
      asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0x1b) : "memory");
    }
    io::out(0x20, 0x11);
    io::wait();
    io::out(0xa0, 0x11);
    io::wait();
    io::out(0x21, 32);
    io::wait();
    io::out(0xa1, 40);
    io::wait();
    io::out(0x21, 4);
    io::wait();
    io::out(0xa1, 2);
    io::wait();
    io::out(0x21, 1);
    io::wait();
    io::out(0xa1, 1);
    io::wait();
    io::out(0x21, 0xff);
    io::out(0xa1, 0xff);
  }

  bool spurious(uint8_t irq) {
    if (irq == 7) {
      io::out(0x20, 0x0b);
      return (io::in(0x20) & 0x80) == 0;
    }
    if (irq == 15) {
      io::out(0xa0, 0x0b);
      if ((io::in(0xa0) & 0x80) == 0) {
        io::out(0x20, 0x20);
        return true;
      }
    }
    return false;
  }
}

extern "C" void loadGdt(const Descriptor* descriptor);
extern "C" const uintptr_t interruptStubs[256];

void interrupts::initialize() {
  io::disableInterrupts();
  task.stacks[0] = reinterpret_cast<uintptr_t>(faultStack + sizeof(faultStack));
  task.ioMap = sizeof(TaskState);
  const uintptr_t address = reinterpret_cast<uintptr_t>(&task);
  gdt[3] = (sizeof(TaskState) - 1) | ((address & 0xffffff) << 16) |
    (uint64_t {0x89} << 40) | (((address >> 24) & 0xff) << 56);
  gdt[4] = address >> 32;
  const Descriptor gdtDescriptor = {sizeof(gdt) - 1, reinterpret_cast<uintptr_t>(gdt)};
  loadGdt(&gdtDescriptor);
  for (size_t index = 0; index < 256; index++) {
    const uintptr_t handler = interruptStubs[index];
    gates[index] = {.low = static_cast<uint16_t>(handler),
                    .selector = 0x08,
                    .stack = static_cast<uint8_t>(index == 8 ? 1 : 0),
                    .attributes = 0x8e,
                    .middle = static_cast<uint16_t>(handler >> 16),
                    .high = static_cast<uint32_t>(handler >> 32),
                    .reserved = 0};
  }
  const Descriptor idt = {.limit = sizeof(gates) - 1,
                          .address = reinterpret_cast<uintptr_t>(gates)};
  asm volatile("lidt %0" : : "m"(idt) : "memory");
  remapPic();
}

bool interrupts::registerIrq(uint8_t irq, Handler handler) {
  if (irq >= 16 || irq == 2 || handler == nullptr) {
    return false;
  }
  const uint64_t flags = io::disableInterrupts();
  if (handlers[irq] != nullptr) {
    io::restoreInterrupts(flags);
    return false;
  }
  handlers[irq] = handler;
  const uint16_t port = irq < 8 ? 0x21 : 0xa1;
  io::out(port, static_cast<uint8_t>(io::in(port) & ~(1U << (irq % 8))));
  if (irq >= 8) {
    io::out(0x21, static_cast<uint8_t>(io::in(0x21) & ~4U));
  }
  io::restoreInterrupts(flags);
  return true;
}

uint64_t interrupts::count(uint8_t irq) {
  return irq < 16 ? __atomic_load_n(&counts[irq], __ATOMIC_RELAXED) : 0;
}

extern "C" void interruptDispatch(const interrupts::Frame* frame) {
  if (frame->vector >= 32 && frame->vector < 48) {
    const uint8_t irq = static_cast<uint8_t>(frame->vector - 32);
    if (spurious(irq)) {
      return;
    }
    __atomic_fetch_add(&counts[irq], 1, __ATOMIC_RELAXED);
    if (handlers[irq] != nullptr) {
      handlers[irq]();
    }
    if (irq >= 8) {
      io::out(0xa0, 0x20);
    }
    io::out(0x20, 0x20);
    return;
  }
  console::printf("\nException %llu: %s\nRIP=%llx RSP=%llx error=%llx\n",
                  static_cast<unsigned long long>(frame->vector),
                  frame->vector < 32 ? exceptions[frame->vector] : "Unexpected vector",
                  static_cast<unsigned long long>(frame->rip),
                  static_cast<unsigned long long>(frame->rsp),
                  static_cast<unsigned long long>(frame->error));
  if (frame->vector == 14) {
    uintptr_t address;
    asm volatile("mov %%cr2, %0" : "=r"(address));
    console::printf("Fault address=%llx (%s, %s, %s)\n",
                    static_cast<unsigned long long>(address),
                    (frame->error & 1) != 0 ? "protection" : "not present",
                    (frame->error & 2) != 0 ? "write" : "read",
                    (frame->error & 4) != 0 ? "user" : "kernel");
  }
  panic("Unhandled CPU exception");
}
