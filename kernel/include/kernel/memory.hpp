#pragma once

#include <stddef.h>
#include <stdint.h>

namespace memory {
  constexpr size_t pageSize = 4096;

  struct PageStats {
    size_t totalPages;
    size_t freePages;
    size_t metadataPages;
    size_t regions;
  };

  class PageAllocator {
   public:
    bool addRegion(void* address, size_t size);
    void* allocate(size_t count = 1);
    bool release(void* address);
    PageStats stats() const;

   private:
    struct Region {
      uint8_t* address;
      size_t pages;
      size_t metadata;
    };
    Region regions[128] = {};
    size_t regionCount = 0;
    size_t freePages = 0;
  };

  struct HeapStats {
    size_t usedBytes;
    size_t freeBytes;
    size_t largestFree;
    size_t allocations;
  };

  class Heap {
   public:
    bool initialize(void* address, size_t size);
    void* allocate(size_t size);
    bool release(void* address);

   private:
    struct alignas(16) Block {
      size_t size;
      Block* previous;
      Block* next;
      bool free;
    };
    Block* first = nullptr;
    void merge(Block* block);
  };

}
