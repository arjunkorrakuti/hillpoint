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

void* memory::Heap::allocate(size_t size) {
  constexpr size_t alignment = alignof(Block);
  if (size == 0 || size > SIZE_MAX - (alignment - 1)) {
    return nullptr;
  }
  size = (size + alignment - 1) & ~(alignment - 1);
  for (Block* block = first; block != nullptr; block = block->next) {
    if (!block->free || block->size < size) {
      continue;
    }
    block->free = false;
    return block + 1;
  }
  return nullptr;
}
