#include <kernel/console.hpp>
#include <kernel/runtime.hpp>

[[noreturn]] void halt() {
  while (true) {
    asm volatile("cli; hlt" ::: "memory");
  }
}

[[noreturn]] void panic(const char* message) {
  asm volatile("cli" ::: "memory");
  console::printf("\nPANIC: %s\n", message);
  halt();
}
