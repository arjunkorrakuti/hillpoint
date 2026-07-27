#include <kernel/memory.hpp>
#include <string.h>

namespace {
  constexpr uint8_t available = 0;
  constexpr uint8_t head = 1;
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

memory::PageStats memory::PageAllocator::stats() const {
  PageStats result = {0, freePages, 0, regionCount};
  for (size_t index = 0; index < regionCount; index++) {
    result.totalPages += regions[index].pages;
    result.metadataPages += regions[index].metadata;
  }
  return result;
}

void* memory::PageAllocator::allocate() {
  for (size_t index = 0; index < regionCount; index++) {
    Region& region = regions[index];
    for (size_t page = region.metadata; page < region.pages; page++) {
      if (region.address[page] == available) {
        region.address[page] = head;
        freePages--;
        return region.address + page * pageSize;
      }
    }
  }
  return nullptr;
}
