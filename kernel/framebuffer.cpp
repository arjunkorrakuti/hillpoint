#include <kernel/framebuffer.hpp>

namespace {
  Framebuffer screen = {};
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
  pixels[y * (screen.pitch / 4) + x] = color;
}
