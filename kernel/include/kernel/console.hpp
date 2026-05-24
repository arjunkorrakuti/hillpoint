#pragma once

#include <kernel/framebuffer.hpp>

namespace console {
  bool initialize(const Framebuffer& framebuffer);
  void write(const char* string, size_t size);
  void write(const char* string);
  void putchar(char character);
}
