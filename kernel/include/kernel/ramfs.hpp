#pragma once

#include <kernel/memory.hpp>

namespace ramfs {
  constexpr size_t maxFiles = 16;
  constexpr size_t maxFileSize = 256;
  enum class Result { ok, invalidName, full, noMemory, tooLarge, notFound, exists };

  struct File {
    char name[32];
    char* data;
    size_t size;
  };

  class Store {
   public:
    void initialize(memory::Heap& allocator);
    Result create(const char* name, const char* data, size_t size);

   private:
    memory::Heap* heap = nullptr;
    File files[maxFiles] = {};
  };

}
