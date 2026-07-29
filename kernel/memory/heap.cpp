#include <kernel/memory.hpp>
#include <stddef.h>
#include <stdint.h>

bool memory::Heap::initialize(void* address, size_t size) {
  if (first != nullptr || address == nullptr ||
      reinterpret_cast<uintptr_t>(address) % alignof(Block) != 0 ||
      size > UINTPTR_MAX - reinterpret_cast<uintptr_t>(address)) {
    return false;
  }
  size -= size % alignof(Block);
  if (size < sizeof(Block) + alignof(Block)) {
    return false;
  }
  first = static_cast<Block*>(address);
  *first = {
    .size = size - sizeof(Block), .previous = nullptr, .next = nullptr, .free = true};
  return true;
}
