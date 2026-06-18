#include <kernel/boot.hpp>
#include <kernel/console.hpp>
#include <kernel/interrupts.hpp>
#include <kernel/runtime.hpp>
#include <kernel/serial.hpp>

extern "C" void kmain() {
  serial::initialize();
  if (!boot::supported()) {
    panic("Unsupported Limine base revision");
  }
  console::initialize(boot::framebuffer());
  interrupts::initialize();
  console::write("Hillpoint\nA small x86-64 kernel\n");
  boot::printSummary();
  halt();
}
