#pragma once

#include <kernel/memory.hpp>

namespace ramfs {
  constexpr size_t maxFiles = 32;
  constexpr size_t maxFileSize = 4096;
  enum class Result { ok, invalidName, full, noMemory, tooLarge, notFound };

  struct File {
    char name[32];
    char* data;
    size_t size;
  };

  class Store {
   public:
    Store() = default;
    Store(const Store&) = delete;
    Store& operator=(const Store&) = delete;
    void initialize(memory::Heap& allocator);
    Result write(const char* name, const char* data, size_t size);
    Result remove(const char* name);
    const File* find(const char* name) const;
    const File* entry(size_t index) const;

   private:
    memory::Heap* heap = nullptr;
    File files[maxFiles] = {};
  };

  const char* message(Result result);
}
