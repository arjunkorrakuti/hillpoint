#include <kernel/ramfs.hpp>
#include <string.h>

void ramfs::Store::initialize(memory::Heap& allocator) {
  if (heap == nullptr) {
    heap = &allocator;
  }
}
