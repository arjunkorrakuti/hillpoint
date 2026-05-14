#include <kernel/framebuffer.hpp>

namespace {
  Framebuffer screen = {};

  uint32_t encode(uint32_t color) {
    return (((color >> 16) & 0xff) << screen.redShift) |
           (((color >> 8) & 0xff) << screen.greenShift) |
           ((color & 0xff) << screen.blueShift);
  }
}

bool graphics::initialize(const Framebuffer& framebuffer) {
  if (framebuffer.address == nullptr || framebuffer.bpp != 32) {
    return false;
  }
  screen = framebuffer;
  return true;
}

void graphics::pixel(size_t x, size_t y, uint32_t color) {
  if (x >= screen.width || y >= screen.height) {
    return;
  }
  auto* pixels = static_cast<volatile uint32_t*>(screen.address);
  pixels[y * (screen.pitch / 4) + x] = encode(color);
}
