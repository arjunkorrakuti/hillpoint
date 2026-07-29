#include <kernel/memory.hpp>
#include <stddef.h>
#include <stdint.h>

bool memory::Heap::initialize(void* address, size_t size) {
  if (address == nullptr || size < sizeof(Block) + alignof(Block) ||
      reinterpret_cast<uintptr_t>(address) % alignof(Block) != 0) {
    return false;
  }
  first = static_cast<Block*>(address);
  *first = {
    .size = size - sizeof(Block), .previous = nullptr, .next = nullptr, .free = true};
  return true;
}
