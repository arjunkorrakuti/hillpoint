#pragma once

#include <kernel/framebuffer.hpp>

namespace console {
  bool initialize(const Framebuffer& framebuffer);
  void putchar(char character);
}
