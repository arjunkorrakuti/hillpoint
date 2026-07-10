#include <kernel/boot.hpp>
#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/keyboard.hpp>
#include <kernel/runtime.hpp>
#include <kernel/serial.hpp>
#include <kernel/timer.hpp>

extern "C" void kmain() {
  serial::initialize();
  if (!boot::supported()) {
    panic("Unsupported Limine base revision");
  }
  console::initialize(boot::framebuffer());
  interrupts::initialize();
  console::write("Hillpoint\nA small x86-64 kernel\n");
  boot::printSummary();
  timer::initialize();
  console::printf("PIT timer: approximately %u Hz.\n", timer::frequency);
  console::printf("PS/2 keyboard: %s\n", keyboard::initialize() ? "ready" : "unavailable");
  asm volatile("sti" ::: "memory");
  while (true) {
    keyboard::Key key;
    if (keyboard::read(key)) {
      console::putchar(static_cast<char>(key));
    }
  }
}
