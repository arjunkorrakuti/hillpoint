#pragma once

#include <kernel/framebuffer.hpp>
#include <stdarg.h>

namespace console {
  bool initialize(const Framebuffer& framebuffer);
  void write(const char* string, size_t size);
  void write(const char* string);
  size_t vprintf(const char* pattern, va_list arguments);
  size_t printf(const char* pattern, ...) __attribute__((format(printf, 1, 2)));
  void clear();
  void putchar(char character);
}
