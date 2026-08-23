#include <kernel/ramfs.hpp>
#include <string.h>

namespace {
  bool validName(const char* name) {
    const size_t size = strlen(name);
    if (size == 0 || size >= 32 || strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
      return false;
    }
    for (size_t index = 0; index < size; index++) {
      const char character = name[index];
      if (!((character >= 'a' && character <= 'z') ||
            (character >= 'A' && character <= 'Z') ||
            (character >= '0' && character <= '9') ||
            character == '.' || character == '-' || character == '_')) {
        return false;
      }
    }
    return true;
  }
}

void ramfs::Store::initialize(memory::Heap& allocator) {
  if (heap == nullptr) {
    heap = &allocator;
  }
}

ramfs::Result ramfs::Store::write(const char* name, const char* data, size_t size) {
  if (!validName(name)) {
    return Result::invalidName;
  }
  if (size > maxFileSize) {
    return Result::tooLarge;
  }
  File* slot = nullptr;
  for (File& file : files) {
    if (file.data != nullptr && strcmp(file.name, name) == 0) {
      slot = &file;
      break;
    }
    if (file.data == nullptr && slot == nullptr) {
      slot = &file;
    }
  }
  if (slot == nullptr) {
    return Result::full;
  }
  auto* replacement = heap != nullptr ? static_cast<char*>(heap->allocate(size + 1))
                                     : nullptr;
  if (replacement == nullptr) {
    return Result::noMemory;
  }
  memcpy(replacement, data, size);
  replacement[size] = '\0';
  heap->release(slot->data);
  memcpy(slot->name, name, strlen(name) + 1);
  slot->data = replacement;
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

const char* ramfs::message(Result result) {
  switch (result) {
    case Result::ok:
      return "ok";
    case Result::invalidName:
      return "Use a name of 1-31 letters, digits, dots, - or _.";
    case Result::full:
      return "File limit reached (16).";
    case Result::noMemory:
      return "Out of heap memory; existing file preserved.";
    case Result::tooLarge:
      return "File exceeds 4096 bytes.";
    case Result::notFound:
      return "File not found.";
  }
  return "Unknown file error.";
}

const ramfs::File* ramfs::Store::entry(size_t index) const {
  return index < maxFiles && files[index].data != nullptr ? &files[index] : nullptr;
}

ramfs::Result ramfs::Store::remove(const char* name) {
  for (File& file : files) {
    if (file.data != nullptr && strcmp(file.name, name) == 0) {
      heap->release(file.data);
      file = {};
      return Result::ok;
    }
  }
  return Result::notFound;
}
