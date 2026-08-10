#include <kernel/boot.hpp>
#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/keyboard.hpp>
#include <kernel/memory.hpp>
#include <kernel/runtime.hpp>
#include <kernel/serial.hpp>
#include <kernel/shell.hpp>
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
  if (!memory::initialize()) {
    panic("Could not initialize physical memory and heap");
  }
  console::printf("Physical page allocator and kernel heap ready.\n");
  timer::initialize();
  console::printf("PIT timer: approximately %u Hz.\n", timer::frequency);
  console::printf("PS/2 keyboard: %s\n", keyboard::initialize() ? "ready" : "unavailable");
  shell::run();
}
