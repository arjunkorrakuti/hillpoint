#include <kernel/interrupts.hpp>
#include <kernel/io.hpp>
#include <kernel/runtime.hpp>
#include <kernel/timer.hpp>

namespace {
  constexpr uint32_t inputFrequency = 1193182;
  constexpr uint16_t divisor = inputFrequency / timer::frequency;
  uint64_t elapsed = 0;

  void tick() {
    __atomic_fetch_add(&elapsed, 1, __ATOMIC_RELAXED);
  }
}

void timer::initialize() {
  io::out(0x43, 0x36);
  io::out(0x40, static_cast<uint8_t>(divisor));
  io::out(0x40, static_cast<uint8_t>(divisor >> 8));
  if (!interrupts::registerIrq(0, tick)) {
    panic("Timer IRQ registration failed");
  }
}

uint64_t timer::ticks() {
  return __atomic_load_n(&elapsed, __ATOMIC_RELAXED);
}

uint64_t timer::milliseconds() {
  return ticks() * 1000 / frequency;
}
