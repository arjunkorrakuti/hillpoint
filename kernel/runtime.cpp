#include <kernel/runtime.hpp>

[[noreturn]] void halt() {
  while (true) {
    asm volatile("cli; hlt" ::: "memory");
  }
}
