#include <kernel/framebuffer.hpp>

namespace {
  Framebuffer screen = {};

  uint32_t encode(uint32_t color) {
    return (((color >> 16) & 0xff) << screen.redShift) |
           (((color >> 8) & 0xff) << screen.greenShift) |
           ((color & 0xff) << screen.blueShift);
  }

  volatile uint32_t* row(size_t y) {
    auto* address = static_cast<uint8_t*>(screen.address);
    return reinterpret_cast<volatile uint32_t*>(address + y * screen.pitch);
  }
}

bool graphics::supported(const Framebuffer& framebuffer) {
  if (framebuffer.address == nullptr || framebuffer.width == 0 ||
      framebuffer.height == 0 || framebuffer.bpp != 32 ||
      framebuffer.width > SIZE_MAX / 4 ||
      framebuffer.pitch < framebuffer.width * 4 || framebuffer.pitch % 4 != 0 ||
      framebuffer.height > SIZE_MAX / framebuffer.pitch ||
      reinterpret_cast<uintptr_t>(framebuffer.address) % 4 != 0 ||
      framebuffer.redSize != 8 || framebuffer.greenSize != 8 ||
      framebuffer.blueSize != 8 || framebuffer.redShift > 24 ||
      framebuffer.greenShift > 24 || framebuffer.blueShift > 24) {
    return false;
  }
  const uint32_t red = 0xffU << framebuffer.redShift;
  const uint32_t green = 0xffU << framebuffer.greenShift;
  const uint32_t blue = 0xffU << framebuffer.blueShift;
  return (red & green) == 0 && (red & blue) == 0 && (green & blue) == 0;
}

bool graphics::initialize(const Framebuffer& framebuffer) {
  const bool valid = supported(framebuffer);
  screen = valid ? framebuffer : Framebuffer{};
  return valid;
}

size_t graphics::width() {
  return screen.width;
}

size_t graphics::height() {
  return screen.height;
}

void graphics::pixel(size_t x, size_t y, uint32_t color) {
  if (x < screen.width && y < screen.height) {
    row(y)[x] = encode(color);
  }
}

void graphics::clear(uint32_t color) {
  scroll(screen.height, color);
}

void graphics::scroll(size_t rows, uint32_t color) {
  if (rows > screen.height) {
    rows = screen.height;
  }
  for (size_t y = 0; y < screen.height - rows; y++) {
    for (size_t x = 0; x < screen.width; x++) {
      row(y)[x] = row(y + rows)[x];
    }
  }
  const uint32_t encoded = encode(color);
  for (size_t y = screen.height - rows; y < screen.height; y++) {
    for (size_t x = 0; x < screen.width; x++) {
      row(y)[x] = encoded;
    }
  }
}
