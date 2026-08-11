#include <kernel/boot.hpp>
#include <kernel/memory.hpp>

namespace {
  constinit memory::PageAllocator allocator;
  constinit memory::Heap kernelHeap;
}

bool memory::initialize() {
  for (size_t index = 0; index < boot::memoryRegions(); index++) {
    const boot::MemoryRegion region = boot::memoryRegion(index);
    if (!region.usable || region.length < pageSize ||
        region.base > UINT64_MAX - region.length ||
        region.base > UINT64_MAX - (pageSize - 1)) {
      continue;
    }
    const uint64_t start = (region.base + pageSize - 1) & ~(uint64_t{pageSize} - 1);
    const uint64_t end = (region.base + region.length) & ~(uint64_t{pageSize} - 1);
    if (end <= start || end - start <= pageSize) {
      continue;
    }
    void* address = boot::directMap(start);
    if (address != nullptr && !allocator.addRegion(address, end - start)) {
      return false;
    }
  }
  for (size_t count = 256; count >= 16; count /= 2) {
    void* arena = allocator.allocate(count);
    if (arena != nullptr) {
      return kernelHeap.initialize(arena, count * pageSize);
    }
  }
  return false;
}

memory::PageAllocator& memory::pages() {
  return allocator;
}

memory::Heap& memory::heap() {
  return kernelHeap;
}
