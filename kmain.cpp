#include <kernel/boot.hpp>
#include <kernel/console.hpp>
#include <kernel/runtime.hpp>

extern "C" void kmain() {
  if (!boot::supported()) {
    halt();
  }
  console::initialize(boot::framebuffer());
  const char* message = "Hillpoint";
  while (*message != '\0') {
    console::putchar(*message++);
  }
  halt();
}
