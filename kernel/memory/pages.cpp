#include <kernel/memory.hpp>
#include <string.h>

namespace {
  constexpr uint8_t available = 0;
  constexpr uint8_t head = 1;
  constexpr uint8_t continuation = 2;
  constexpr uint8_t reserved = 3;
}

bool memory::PageAllocator::addRegion(void* address, size_t size) {
  const uintptr_t start = reinterpret_cast<uintptr_t>(address);
  if (address == nullptr || start % pageSize != 0 ||
      size > UINTPTR_MAX - start || regionCount == 128) {
    return false;
  }
  const size_t count = size / pageSize;
  const size_t metadata = count / pageSize + (count % pageSize != 0 ? 1 : 0);
  if (count <= metadata) {
    return false;
  }
  for (size_t index = 0; index < regionCount; index++) {
    const uintptr_t other = reinterpret_cast<uintptr_t>(regions[index].address);
    if (start < other + regions[index].pages * pageSize && other < start + size) {
      return false;
    }
  }
  auto* bytes = static_cast<uint8_t*>(address);
  memset(bytes, available, count);
  memset(bytes, reserved, metadata);
  regions[regionCount++] = {bytes, count, metadata};
  freePages += count - metadata;
  return true;
}

void* memory::PageAllocator::allocate(size_t count) {
  if (count == 0 || count > freePages) {
    return nullptr;
  }
  for (size_t index = 0; index < regionCount; index++) {
    Region& region = regions[index];
    size_t run = 0;
    for (size_t page = region.metadata; page < region.pages; page++) {
      run = region.address[page] == available ? run + 1 : 0;
      if (run == count) {
        const size_t first = page + 1 - count;
        region.address[first] = head;
        memset(region.address + first + 1, continuation, count - 1);
        freePages -= count;
        void* result = region.address + first * pageSize;
        memset(result, 0, count * pageSize);
        return result;
      }
    }
  }
  return nullptr;
}

bool memory::PageAllocator::release(void* address) {
  const uintptr_t pointer = reinterpret_cast<uintptr_t>(address);
  for (size_t index = 0; index < regionCount; index++) {
    Region& region = regions[index];
    const uintptr_t start = reinterpret_cast<uintptr_t>(region.address);
    if (pointer < start || pointer - start >= region.pages * pageSize ||
        (pointer - start) % pageSize != 0) {
      continue;
    }
    size_t page = (pointer - start) / pageSize;
    if (region.address[page] != head) {
      return false;
    }
    do {
      region.address[page++] = available;
      freePages++;
    } while (page < region.pages && region.address[page] == continuation);
    return true;
  }
  return false;
}

memory::PageStats memory::PageAllocator::stats() const {
  PageStats result = {0, freePages, 0, regionCount};
  for (size_t index = 0; index < regionCount; index++) {
    result.totalPages += regions[index].pages;
    result.metadataPages += regions[index].metadata;
  }
  return result;
}
