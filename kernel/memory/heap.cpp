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
    if (block->size - size >= sizeof(Block) + alignment) {
      auto* remainder =
        reinterpret_cast<Block*>(reinterpret_cast<uint8_t*>(block + 1) + size);
      *remainder = {.size = block->size - size - sizeof(Block),
                    .previous = block,
                    .next = block->next,
                    .free = true};
      if (remainder->next != nullptr) {
        remainder->next->previous = remainder;
      }
      block->next = remainder;
      block->size = size;
    }
    block->free = false;
    return block + 1;
  }
  return nullptr;
}

bool memory::Heap::release(void* address) {
  if (address == nullptr) {
    return true;
  }
  for (Block* block = first; block != nullptr; block = block->next) {
    if (block + 1 != address) {
      continue;
    }
    if (block->free) {
      return false;
    }
    block->free = true;
    merge(block);
    return true;
  }
  return false;
}

void memory::Heap::merge(Block* block) {
  Block* next = block->next;
  if (next != nullptr && next->free) {
    block->size += sizeof(Block) + next->size;
    block->next = next->next;
    if (block->next != nullptr) {
      block->next->previous = block;
    }
  }
}
