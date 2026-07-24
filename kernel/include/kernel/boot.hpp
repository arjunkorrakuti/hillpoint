#pragma once

#include <kernel/framebuffer.hpp>

namespace boot {
  struct MemoryRegion {
    uint64_t base;
    uint64_t length;
    bool usable;
  };

  bool supported();
  Framebuffer framebuffer();
  void printSummary();
  size_t memoryRegions();
  MemoryRegion memoryRegion(size_t index);
  void* directMap(uint64_t physical);
}
