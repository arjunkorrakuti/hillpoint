#include <kernel/ramfs.hpp>
#include <string.h>

void ramfs::Store::initialize(memory::Heap& allocator) {
  if (heap == nullptr) {
    heap = &allocator;
  }
}

ramfs::Result ramfs::Store::create(const char* name, const char* data, size_t size) {
  const size_t length = strlen(name);
  if (length == 0 || length >= 32) {
    return Result::invalidName;
  }
  if (size > maxFileSize) {
    return Result::tooLarge;
  }
  File* slot = nullptr;
  for (File& file : files) {
    if (file.data != nullptr && strcmp(file.name, name) == 0) {
      return Result::exists;
    }
    if (file.data == nullptr && slot == nullptr) {
      slot = &file;
    }
  }
  if (slot == nullptr) {
    return Result::full;
  }
  auto* contents = heap != nullptr ? static_cast<char*>(heap->allocate(size + 1))
                                  : nullptr;
  if (contents == nullptr) {
    return Result::noMemory;
  }
  memcpy(contents, data, size);
  contents[size] = '\0';
  memcpy(slot->name, name, length + 1);
  slot->data = contents;
  slot->size = size;
  return Result::ok;
}

const ramfs::File* ramfs::Store::find(const char* name) const {
  for (const File& file : files) {
    if (file.data != nullptr && strcmp(file.name, name) == 0) {
      return &file;
    }
  }
  return nullptr;
}
