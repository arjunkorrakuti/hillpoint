#pragma once

#include <stddef.h>
#include <stdint.h>

struct Framebuffer {
  void* address;
  size_t width;
  size_t height;
  size_t pitch;
  uint16_t bpp;
  uint8_t redSize;
  uint8_t redShift;
  uint8_t greenSize;
  uint8_t greenShift;
  uint8_t blueSize;
  uint8_t blueShift;
};


namespace graphics {
  bool supported(const Framebuffer& framebuffer);
  bool initialize(const Framebuffer& framebuffer);
  size_t width();
  size_t height();
  void clear(uint32_t color);
  void pixel(size_t x, size_t y, uint32_t color);
}
